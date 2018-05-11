#!/bin/bash
sudo apt-get update &&
sudo apt-get install -y git &&

# install IncludeOS
pushd $HOME &&
git clone --recurse-submodules https://github.com/hioa-cs/IncludeOS &&
popd &&
pushd $HOME/IncludeOS &&
sudo ./install.sh -y &&
popd &&

# install libvirt
sudo apt-get install -y libvirt-bin virtinst &&
#NB: requires logout/login to take effect
sudo usermod -a -G libvirtd $USER &&

# install Docker
sudo apt-get install -y apt-transport-https ca-certificates curl software-properties-common &&
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add - &&
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" &&
sudo apt-get update &&
sudo apt-get install -y docker-ce &&
sudo usermod -a -G docker $USER &&

echo "#############################################################################"
echo "###                                                                       ###"
echo "###  /!\  You must logout/login to apply changes before continuing.  /!\  ###"
echo "###                                                                       ###"
echo "#############################################################################"
