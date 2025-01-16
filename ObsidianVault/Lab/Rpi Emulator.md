


# Install Docker

read the docs :)
https://docs.docker.com/engine/install/

# First run
- go to a directory where you want your disk image to live.
- For me, that is ~/teaching/lab
- 
```
cd ~/teaching/demo_lab
docker run --rm -it -v $PWD/dist:/dist ptrsr/pi-ci start
```

Once you get a shell, power off the emulator by running
```
systemctl poweroff
```

 Make sure that the container has stopped
 ```
docker ps  
```
then resize the disk image 
```
docker run --rm -it -v $PWD/dist:/dist ptrsr/pi-ci resize 10G
```
Finally, boot up and forward ssh 
```
docker run --rm -it -p 127.0.0.1:2222:2222 -v $PWD/dist:/dist ptrsr/pi-ci start
```
In another terminal, run 
```
ssh root@127.0.0.1 -p 2222 -o StrictHostKeyChecking=no
```

```
sudo apt update && sudo apt install -y zsh git curl build-essential gdb tmux 
```
I also recommend installing `magic-wormhole` on both guest and host. SCP works fine too. 
