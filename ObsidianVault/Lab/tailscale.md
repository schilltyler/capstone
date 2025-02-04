# HOWTO: Setting Up Remote Access to Your Raspberry Pi with Tailscale

This guide will help you set up your Raspberry Pi lab for remote access using Tailscale. Tailscale is a peer-to-peer Virtual Private Network (VPN) tool that simplifies networking by allowing devices to securely connect to each other as if they're on the same local network.

## What is Tailscale?

Tailscale creates a secure, private network (a "tailnet") that connects your devices using WireGuard, a fast and modern VPN protocol. With Tailscale, you can:

- Connect your devices from anywhere.
- SSH into your Raspberry Pi easily.
- Access your devices without worrying about NAT, firewalls, or port forwarding.
- Collaborate securely with others on the same tailnet.

## Steps to Set Up Tailscale for Your Raspberry Pi Lab

### 1. Pre-requisites

1. **Receive your Tailscale Preauth Key:** After signing up for the CTF, you'll receive a Discord DM with 10 Tailscale preauth keys to authenticate to the student VPN.
2. **Login URL:** Use the provided login URL: [https://vpn.ch0nky.com](https://vpn.ch0nky.com).
3. **Devices to Connect:** You need to connect:
   - Your main machine (e.g., laptop or desktop).
   - Your Raspberry Pi (with SSH enabled).

### 2. Install Tailscale on Your Linux System

#### On Your Main Machine (Linux)
1. **Update your system:**
   ```bash
   sudo apt update && sudo apt upgrade -y
```

2. **Install Tailscale:**
    
    ```bash
    curl -fsSL https://tailscale.com/install.sh | sh
    ```
    
3. **Start the Tailscale service:**
    
    ```bash
    sudo tailscale up --login-server https://vpn.ch0nky.com --auth-key <your-preauth-key>
    ```
    

#### On Your Raspberry Pi

1. **Update your Raspberry Pi:**
    
    ```bash
    sudo apt update && sudo apt upgrade -y
    ```
    
2. **Install Tailscale:**
    
    ```bash
    curl -fsSL https://tailscale.com/install.sh | sh
    ```
    
3. **Start Tailscale with SSH enabled:**
    
    ```bash
    sudo tailscale up --login-server https://vpn.ch0nky.com --auth-key <your-preauth-key> --ssh
    ```
    

### 3. Verify Connection to the Tailnet

- Run the following command on your main machine and Raspberry Pi to check connected devices:
    
    ```bash
    tailscale status
    ```
    
    You should see both devices listed on the tailnet.

### 4. SSH Into Your Raspberry Pi

1. On your main machine, use the following command to SSH into your Raspberry Pi:
    
    ```bash
    ssh user@<your-raspberry-pi-tailscale-ip>
    ```
    
    Replace `<your-raspberry-pi-tailscale-ip>` with the Tailscale-assigned IP address for your Raspberry Pi (visible in `tailscale status`).
    
2. If needed, use this method to troubleshoot or configure your Raspberry Pi remotely.
    

### 5. Support and Troubleshooting

- **Instructor Access:** Your instructor can SSH into your Raspberry Pi for support if needed.
- **Reconnect Devices:** If a device disconnects, rerun the `tailscale up` command with the same preauth key.
- **Tailscale Commands:**
    - Restart Tailscale: `sudo systemctl restart tailscaled`
    - Check Tailscale logs: `sudo journalctl -u tailscaled`

### Additional Notes

- Keep your preauth keys secure. Do not share them with others.
- You can connect up to 10 devices with the preauth keys you received.
- each preauth key is single use and expires at the end of the course. You may use each key once. If you lose your keys, you may request more, but will lose 2.5% off of your final grade. So don't lose your keys.
- For further details, visit [Tailscale Documentation](https://tailscale.com/kb/).

Now you're all set to remotely manage and access your Raspberry Pi securely!

