
## 1. Security Rant (Do not skip)

Regardless of whether you are analyzing Linux or Windows malware:

- **Always** analyze unknown or malicious software in an **isolated sandbox**.
- **Do not** allow malicious code to access any sensitive host resources, local network shares, or the broader internet (unless strictly needed for your analysis).
- **Virtualization** is not a magic bullet—hypervisor escapes exist. You won't encounter any in this class, but you should probably avoid using this setup for real world malware.

---

## 2. High-Level Overview

We will deploy two Linux virtual machines via **KVM + libvirt**:

1. **Dirty VM** hostname `jail`
    - A minimal Linux OS (Ubuntu, Debian, or any distro of choice).
    - Where you execute and analyze malicious Linux binaries.
    - Network traffic is optionally **routed** through the Gateway VM.
    - Initially setup with two nics  (nat and private)
    - nat  is used to update and install tools. after, the nic internet access is removed from the guest
    - the nic can be added back at any time if more tools are needed
1. **Gateway VM** hostname `gateway`:
    - A minimal Linux OS that serves as the traffic inspection point (using tools like `tcpdump`, `Wireshark`, `mitmproxy`, or `inetsim`).
    - Has **two NICs**:
        - **Private Network** interface shared only with the Dirty VM.
        - **NAT or bridged** interface that can optionally connect out to the internet.
    - Forwards traffic from Dirty to the internet (if desired) and can simulate local services (DNS, HTTP, etc.) using something like **inetsim**.

### Network Diagram
(note actual interface names might be differnt)
```
[Host/Raspberry Pi]
        |
        | (KVM + libvirt)
        |
     +----------+
     | Gateway  |  <-- (VM #1)
     |  eth0: NAT or Bridge -> Internet
     |  eth1: Private -> 10.10.10.2/24
     +----------+
          ^
          | (Private Network: 10.10.10.0/24)
          |
     +----------+
     |  Dirty   |  <-- (VM #2)
     |  eth0: Private -> 10.10.10.3/24
     +----------+
```

---

## 3. KVM + libvirt on Your Host (e.g., Raspberry Pi)

Before provisioning anything, ensure that:

1. **KVM + libvirt** are installed, and your user is in the `libvirt` (and possibly `kvm`) group.
2. You have **enough CPU / RAM** resources on your Pi to run at least two simultaneous VMs.
3. You have a **libvirt network** configured for NAT or bridging.
    - If bridging, set up a Linux bridge device on the Pi that can talk to your LAN or external interface.
    - If NAT, libvirt can automatically manage NAT via `virbr0`.

On an ubuntu/raspbery pios run
```
sudo apt update && sudo apt install -y \ 
	qemu-kvm \
	libvirt-daemon-system \
	virt-manager \
	cpu-checker \
	libguestfs-tools

# optional
sudo adduser $USER libvirt

```

---

## 4. Creating Two libvirt Networks

We want:

1. **Private, isolated network**:    
    - Used only between Dirty and Gateway.
    - No outside connectivity, purely an internal LAN (e.g., `10.10.10.0/24`).
2. **NAT or bridged network** (for Gateway):
    - So that Gateway can optionally reach the open internet.

### Example: Defining a Private Network

You can define a private network XML (e.g., `private-net.xml`):

```xml
<network>
  <name>malware-private</name>
  <forward mode="none"/>
  <ip address="10.10.10.1" netmask="255.255.255.0">
  </ip>
</network>
```

Then load it:

```bash
sudo virsh net-define private-net.xml
sudo virsh net-start malware-private
sudo virsh net-autostart malware-private
```

```
# activate default network 
$ sudo virsh net-start default 

# verify output
$ sudo virsh net-list --all    


```
verify that the network as been added
```
$ ip a
... snip ...
6: virbr1: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default qlen 1000
    link/ether 52:54:00:66:3e:fa brd ff:ff:ff:ff:ff:ff
    inet 10.10.10.1/24 brd 10.10.10.255 scope global virbr1
       valid_lft forever preferred_lft forever
7: virbr0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default qlen 1000
    link/ether 52:54:00:14:b8:ab brd ff:ff:ff:ff:ff:ff
    inet 192.168.122.1/24 brd 192.168.122.255 scope global virbr0
       valid_lft forever preferred_lft forever


```
(Replace names/addresses as desired.)

### Example: Using the Default NAT

Most libvirt installs come with a default NAT network (`default`), typically `192.168.122.0/24`. You can use that for the Gateway’s external NIC, or you can create a bridged network if you prefer direct LAN access.

---

## 5. VM Installation: Gateway
we make use of cloud init images to allow for easy configuration of of newly created machines. For more on cloud init, see https://cloud-init.io/. All you need to know for this tutorial is the configuration for each machine is stored in yaml files.
https://github.com/Ch0nkyLTD/aarch64-linux-lab/tree/main/lab/jail
https://github.com/Ch0nkyLTD/aarch64-linux-lab/tree/main/lab/gateway
see `setup.sh` and `*_iso.sh`
- to tart from scratch, run `oops.sh` to delete all course Vms and associated storage
```
# download a cloud init image from canonical 
# make a snapshot 

wget https://cloud-images.ubuntu.com/daily/server/jammy/current/jammy-server-cloudimg-arm64.img
qemu-img create -f qcow2 -b jammy-server-cloudimg-arm64.img -F qcow2 gateway.qcow2 20G

 qemu-img create -f qcow2 -b jammy-server-cloudimg-arm64.img -F qcow2 detonate.qcow2 20G

```
```
# now run ls you should see the new images
ls
detonate.qcow2  gateway.qcow2  jammy-server-cloudimg-arm64.img


sudo virsh start gateway
sudo virsh console gateway

 sudo virsh domifaddr gateway 
 Name       MAC address          Protocol     Address
-------------------------------------------------------------------------------
 vnet2      52:54:00:71:df:65    ipv4         192.168.122.43/24




shutdown gateway
Domain 'gateway' is being shutdown

virsh #  undefine gateway --nvram

```

Now generate an ssh key for use with cloud init

```
$ ssh-keygen -t ed25519
Generating public/private ed25519 key pair.
Enter file in which to save the key (/home/user/.ssh/id_ed25519): 
Enter passphrase (empty for no passphrase): 
Enter same passphrase again: 
Your identification has been saved in /home/user/.ssh/id_ed25519
Your public key has been saved in /home/user/.ssh/id_ed25519.pub
The key fingerprint is:
SHA256:TotC2HoheWmTMt5iJNirozHWdRKD8clafuZ92eyCEd4 user@spr-2025
The key's randomart image is:
$ cat /home/user/.ssh/id_ed25519.pub
ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIDhDN7Dt0kAEvk1aPJmBF2s77JEK906XjaBlEmbrgwgm user@spr-2025
```


1. **Download** an Ubuntu/Debian ISO (server or minimal).
    
2. **Create** a VM in libvirt (using `virt-install` or a GUI like `virt-manager`):

# 
detonate.qcow2  gateway.qcow2  jammy-server-cloudimg-arm64.img  private-net.xml  user-data
copy the imaages to `/var/lib/libvirt/images/`


```
sudo cp detonate.qcow2 gateway.qcow2 jammy-server-cloudimg-arm64.img /var/lib/libvirt/images 

```

```shell
#!/usr/bin/env bash


vm_name='gateway'
vm_memory='2048'
vm_cpus='2'
vm_disk='/var/lib/libvirt/images/gateway.qcow2'

ci_user_data='user-data'


virt-install \
    --connect qemu:///system \
    --name "$vm_name" \
    --memory "$vm_memory" \
    --machine q35 \
    --vcpus "$vm_cpus" \
    --cpu host-passthrough \
    --import \
    --cloud-init user-data="$ci_user_data" \
    --osinfo name=almalinux8 \
    --disk "$vm_disk" \
    --virt-type kvm
```


```bash
    virt-install \
      --name gateway \
      --memory 2048 \
      --vcpus 2 \
      --disk size=20,path=/var/lib/libvirt/images/gateway.qcow2 \
      --cdrom /path/to/ubuntu-server.iso \
      --network network=default,model=virtio \
      --network network=malware-private,model=virtio \
      --os-variant ubuntu22.04 \
      --graphics vnc
    ```
    
    - NIC #1 = `network=default` (NAT or bridged)
    - NIC #2 = `network=malware-private` (isolated)
3. **Install** the OS as usual.
    
4. **Configure** a static IP on the private interface (e.g., `10.10.10.2/24`).
    
5. **Install** analysis tools (e.g., `wireshark`, `tcpdump`, `mitmproxy`, `inetsim`) or anything else you need.
    
6. **Enable forwarding** if you want to let traffic from the Dirty VM reach the internet. In `/etc/sysctl.conf`, enable:
    
    ```
    net.ipv4.ip_forward=1
    ```
    
    Then set up an iptables / nftables rule to NAT from the private network to your Gateway’s internet interface. For iptables, e.g.:
    
    ```bash
    # Example iptables script for NAT
    iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
    iptables -A FORWARD -i eth1 -j ACCEPT
    ```
    
    (Adjust interface names as needed.)
    
7. Optionally, **run inetsim** on the Gateway to simulate local DNS/HTTP/FTP, etc., for contained malware analysis.
    

---

## 6. VM Installation: Dirty

1. **Create** another Linux VM (similar steps, but smaller resources) and attach it only to the private network:
    
    ```bash
    virt-install \
      --name dirty \
      --memory 1024 \
      --vcpus 1 \
      --disk size=10,path=/var/lib/libvirt/images/dirty.qcow2 \
      --cdrom /path/to/ubuntu-minimal.iso \
      --network network=malware-private,model=virtio \
      --os-variant ubuntu22.04 \
      --graphics vnc
    ```
    
    - Only one interface on `malware-private`.
2. **Configure** the static IP to be `10.10.10.3/24` (gateway = `10.10.10.2`, DNS = `10.10.10.2`).
    
3. **Install** your usual Linux analysis or dev tools. Or keep it minimal if you plan to scp binaries in for detonation.
    
4. This is where you launch and run suspicious Linux binaries. All traffic goes **through the Gateway**.
    

> **Snapshot** your VM once configured. If it gets corrupted or compromised, revert quickly.

---

## 7. Provisioning with Ansible

Instead of doing all these steps manually, you can automate installation & configuration with Ansible. For example:

1. **Create** an Ansible inventory file (`inventory.ini`):
    
```ini
[jail]
jail ansible_host=10.10.10.11 ansible_user=user

[gateway]
gateway ansible_host=10.10.10.10 ansible_user=user    
```
    
2. **Define** roles/playbooks:
    (see playbook.yml)
3. **Run** your playbooks:
    
    ```bash
ansible-playbook playbook.yml -i inventory.ini
    ```
    

Use whichever approach you prefer: 100% manual or automated with Ansible.


Now ssh into your gateway `ssh user@10.10.10.10` and configure inetsim

```
$ sudo vim /etc/inetsim/inetsim.conf
...
#########################################
# service_bind_address
#
# IP address to bind services to
#
# Syntax: service_bind_address <IP address>
#
# Default: 127.0.0.1
#
service_bind_address 10.10.10.10

....
#########################################
# dns_default_ip
#
# Default IP address to return with DNS replies
#
# Syntax: dns_default_ip <IP address>
#
# Default: 127.0.0.1
#
dns_default_ip 10.10.10.10
```

```
# restart inetsim and verify 
$ sudo systemctl restart inetsim.service
$ sudo ss -tulnp
```

Copy the   bridge script to the gateway and the isolate script to the jail

```
 scp  -i ~/.ssh/id_ed25519 bridge_mal.sh user@10.10.10.10:/home/user/
 scp  -i ~/.ssh/id_ed25519 toggle_isolate.sh user@10.10.10.11:/home/user/
```
On the gateway, you can  set whether or not to forward traffic to the internet with 
```
sudo ./bridge_mal.sh start/stop
```
and  on jail with 
```
sudo ./toggle_isolate isolate/undo
```
For added security, you can fully remove the nic using virsh.

```bash
$ sudo virsh domiflist jail                                             
 Interface   Type      Source            Model    MAC
---------------------------------------------------------------------
 vnet2       network   nat-net           virtio   52:54:00:9b:5a:40
 vnet3       network   malware-private   virtio   52:54:00:77:65:cd
```
Note that in my case, the nic mac address is `52:54:00:9b:5a:40`
to remove, run 
```
sudo virsh detach-interface --domain jail --type network --mac 52:54:00:9b:5a:40 --config --live
```
Now even if  malware were to undo `isolate` it would still be unable to reach the internet
To attach it back run. 

```
 sudo virsh attach-interface --domain jail --type network --source nat-net --model virtio --mac 52:54:00:9b:5a:40 --config --live
```
I recommend leaving it detached and relying on bridging from `gateway`.


---

## 9. Testing Your Setup

1. **Start** the Gateway VM and confirm it has internet connectivity on its primary NIC.
2. **Start** the Dirty VM `jail` and confirm it can only talk to the Gateway (`ping 10.10.10.10`), not the outside world.
3. If you want to allow the Dirty VM to reach the web, turn on MASQUERADE in iptables (on the Gateway). Use the helper script to do this.
4. finally, download the test implant and run it with  tshark on gateway with `jail` disconnected from the internet. 

---

## 10. Final Tips & Disclaimers

- **Snapshot** your VMs often.
- Keep your **host** operating system locked down.
- Assume sophisticated malware _can_ break out of a VM in rare circumstances—especially if you misconfigure the environment.
- Avoid “bidirectional” drag & drop or seamless file sharing. If you must copy files, do it read-only or via a controlled method (e.g., an SFTP server running on the Gateway).
- For advanced Linux malware analysis, tools such as [REMnux](https://remnux.org/) or [Tsurugi Linux](https://tsurugi-linux.org/) can be used as an analysis VM as well.

---

## Optional
Install `sshfs` on the hypervisor to make copying files easier. This adds some attack surface, as does not fully isolating the host from the private network, but in this class, these are acceptable risks. 
```
sudo apt install sshfs

sudo mkdir -p /mnt/jail /mnt/gateway
sudo chown user:user jail
sudo chown user:user gateway
sudo groupadd fuse
sudo usermod -a -G fuse  user

sshfs  -o IdentityFile=~/.ssh/id_ed25519 user@10.10.10.10:/home/user/ /mnt/gateway -o rw,noexec
sshfs -o IdentityFile=~/.ssh/id_ed25519 user@10.10.10.11:/home/user/ /mnt/jail -o rw,noexec


# confirm
└[/mnt]> l jail 
total 264K
drwxr-x--- 1 user fuse 4.0K Jan 29 12:34 .
drwxr-xr-x 5 root root 4.0K Jan 29 12:22 ..
drwx------ 1 user fuse 4.0K Jan 29 12:18 .ansible
-rw------- 1 user fuse   14 Jan 29 12:32 .bash_history
-rw-r--r-- 1 user fuse  220 Jan  6  2022 .bash_logout
-rw-r--r-- 1 user fuse 3.7K Jan  6  2022 .bashrc
drwx------ 1 user fuse 4.0K Jan 29 12:18 .cache
drwxr-xr-x 1 user fuse 4.0K Jan 29 12:21 .oh-my-zsh
-rw-r--r-- 1 user fuse  807 Jan  6  2022 .profile
drwx------ 1 user fuse 4.0K Jan 29 12:18 .ssh
-rw-rw-r-- 1 user fuse  48K Jan 29 12:34 .zcompdump
-rw-rw-r-- 1 user fuse  50K Jan 29 12:34 .zcompdump-ch0nkyjail-5.8.1
-r--r--r-- 1 user fuse 115K Jan 29 12:34 .zcompdump-ch0nkyjail-5.8.1.zwc
-rw------- 1 user fuse  106 Jan 29 12:34 .zsh_history
-rw-r--r-- 1 user fuse 4.0K Jan 29 12:21 .zshrc
┌[user@spr-2025] [/dev/pts/6] 
└[/mnt]> l gateway 
total 40K
drwxr-x--- 1 user fuse 4.0K Jan 29 12:21 .
drwxr-xr-x 5 root root 4.0K Jan 29 12:22 ..
drwx------ 1 user fuse 4.0K Jan 29 12:18 .ansible
-rw-r--r-- 1 user fuse  220 Jan  6  2022 .bash_logout
-rw-r--r-- 1 user fuse 3.7K Jan  6  2022 .bashrc
drwx------ 1 user fuse 4.0K Jan 29 12:18 .cache
drwxr-xr-x 1 user fuse 4.0K Jan 29 12:21 .oh-my-zsh
-rw-r--r-- 1 user fuse  807 Jan  6  2022 .profile
drwx------ 1 user fuse 4.0K Jan 29 12:09 .ssh
-rw-r--r-- 1 user fuse 4.0K Jan 29 12:21 .zshrc
```

- note that each time you remake your vm, the host key will change. to remove it form the hypervisor run ` ssh-keygen -f "/home/user/.ssh/known_hosts" -R "10.10.10.11"` or ` ssh-keygen -f "/home/user/.ssh/known_hosts" -R "10.10.10.10"` for the gateway
- alternatively, you can connect without host key checking. this is unsafe in general, but again, is OK for our purposes. 
- `ssh  -i ~/.ssh/id_ed25519 user@10.10.10.11 -o StrictHostKeyChecking=no`
## Conclusion

By combining **KVM + libvirt** with **Ansible** provisioning, you can create a reliable **two-VM malware lab** for **Linux-based** analysis. The “Gateway” routes, inspects, or simulates services, while the “Dirty” VM is isolated on a private network. Traffic bridging is easily configured via scripts (or netplan), letting you choose whether malware can reach the outside world or remain captive to your analysis environment.

Use snapshots, maintain strict isolation, and proceed with caution whenever detonating unknown code. Good luck, and happy (safe) hacking!