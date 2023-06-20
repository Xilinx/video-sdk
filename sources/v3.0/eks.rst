:orphan:

.. _building-eks-ami:

#################################
Building an EKS AMI for the |SDK|
#################################

.. highlight:: none

This page describes how to create a custom EKS AMI with the |SDK|.

************
Instructions
************

#. Install and configure the AWS Client. On the development machine, i.e., the machine that initiates the AMI build, install `AWS CLI <https://docs.aws.amazon.com/cli/latest/userguide/getting-started-install.html>`__ and `Configure AWS CLI <https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-configure.html>`__

#. Install `Packer <https://www.packer.io/downloads>`_ on the development machine

#. Install the resources and configuration scripts for building a custom Amazon EKS AMI (see `Amazon EKS AMI <https://github.com/awslabs/amazon-eks-ami>`_ for details)::

      git clone https://github.com/awslabs/amazon-eks-ami.git

#. Edit the :file:`eks-worker-al2-variables.json` file and update the ``source_ami_id`` field to the desired value::

      vim eks-worker-al2-variables.json

#. Build the AMI for the target EKS version, e.g., 1.24::

      make 1.24

#. Create the following Packer YAML file, updating the ``variables`` section as appropriate:

   .. code-block:: yaml

      {
       "variables": {
           "region": "[replace with target region]",
           "flag": "eks-1.24",
           "subnet_id": "[replace with your subnet_id]",
           "security_groupids": "[replace with your security_groupids]",
           "build_ami": "[replace with the AMI ID build]"
       },
       "builders": [
           {
               "type": "amazon-ebs",
               "region": "{{user `region`}}",
               "source_ami": "{{user `build_ami`}}",
               "run_tags": {
                   "Name": "packer-vt-processor-{{user `flag`}}"
               },
               "subnet_id": "{{user `subnet_id`}}",
               "security_group_ids": "{{user `security_groupids`}}",
               "instance_type": "vt1.3xlarge",
               "ssh_username": "ec2-user",
               "ami_name": "[Name of Your AMI]",
               "launch_block_device_mappings": [
                   {
                       "delete_on_termination": true,
                       "device_name": "/dev/xvda",
                       "volume_size": 200,
                       "throughput": 1000,
                       "iops": 8000,
                       "volume_type": "gp3"
                   }
               ]
           }
       ],
       "provisioners": [
           {
               "type": "shell",
               "expect_disconnect": true,
               "inline": [
                   "sudo yum update -y",
                   "sudo amazon-linux-extras install lustre2.10 epel -y",
                   "sudo yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm -y",
                   "sudo yum install boost-devel -y",
                   "sudo yum install gcc-c++ -y",
                   "sudo yum install yum-utils cmake sipcalc dkms mdadm git htop hwloc iftop kernel-tools rpm-build rpmdevtools numactl parallel pigz python2-pip python3-pip wget kernel-devel-$(uname -r) kernel-headers-$(uname -r) -y",
                   "sudo yum groupinstall 'Development Tools' -y",
                   "uname -r",
                   "sudo shutdown -r now"
               ]
           },
           {
               "type": "shell",
               "inline_shebang": "/bin/bash -xe",
               "inline": [
                   "echo '[Amzn2Artifactory]' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'name=Amzn2Artifactory' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'baseurl=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/2' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'enabled=1' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'gpgcheck=0' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/repodata/repomd.xml.key' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'sslverify=0' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo '[Artifactory]' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'name=Artifactory' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'baseurl=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'enabled=1' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'gpgcheck=0' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/repodata/repomd.xml.key' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'sslverify=0' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo '[BaseArtifactory]' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'name=BaseArtifactory' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'baseurl=https://packages.xilinx.com/artifactory/rpm-packages/noarch/' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'enabled=1' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'gpgcheck=0' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/noarch/repodata/repomd.xml.key' | sudo tee -a /etc/yum.repos.d/xilinx.repo",
                   "echo 'sslverify=0' | sudo tee -a /etc/yum.repos.d/xilinx.repo"
               ]
           },
           {
               "type": "shell",
               "inline_shebang": "/bin/bash -xe",
               "inline": [
                   "echo '    StrictHostKeyChecking no' | sudo tee -a /etc/ssh/ssh_config",
                   "echo '    HostbasedAuthentication no' | sudo tee -a /etc/ssh/ssh_config",
                   "echo '    CheckHostIP no' | sudo tee -a /etc/ssh/ssh_config"
               ]
           },
           {
               "type": "shell",
               "inline_shebang": "/bin/bash -xe",
               "inline": [
                   "sudo yum update -y",
                   "sudo yum install kernel-devel-$(uname -r) -y",
                   "sudo yum install kernel-headers-$(uname -r) -y",
                   "sudo yum install yum-plugin-versionlock -y",
                   "sudo yum install xrt-2.11.722-1.x86_64 -y",
                   "sudo yum versionlock xrt-2.11.722 -y",
                   "sudo yum install xilinx-alveo-u30-core -y",
                   "sudo yum install xilinx-alveo-u30-ffmpeg -y",
                   "sudo yum install xilinx-alveo-u30-gstreamer -y",
                   "sudo yum install xilinx-alveo-u30-examples -y",
                   "sudo cp /opt/xilinx/xcdr/xclbins/transcode.xclbin /opt/xilinx/xcdr/xclbins/transcode_lite.xclbin",
                   "sudo cp /opt/xilinx/xcdr/xclbins/on_prem/transcode.xclbin /opt/xilinx/xcdr/xclbins/transcode.xclbin",
                   "sudo rm /opt/xilinx/firmware/sc-fw/u30/sc-fw-u30-6.3.8-fe416dd3cea1ac123bfe32afdfb9b9e9.txt",
                   "sudo rm /opt/xilinx/xrt/share/fw/*",
                   "sudo rm /opt/xilinx/firmware/u30/gen3x4/base/data/BOOT_golden.BIN",
                   "sudo rm $HOME/.ssh/authorized_keys"
               ]
           },
           {
               "type": "shell",
               "inline_shebang": "/bin/bash -xe",
               "inline": [
                   "distribution=$(. /etc/os-release;echo $ID$VERSION_ID)",
                   "sudo amazon-linux-extras install docker",
                   "sudo systemctl enable docker",
                   "echo -e '{\"default-ulimits\":{\"memlock\":{\"Name\":\"memlock\",\"Soft\":-1,\"Hard\":-1}}}' | sudo tee /etc/docker/daemon.json",
                   "sudo systemctl restart docker",
                   "sudo usermod -aG docker ec2-user"
               ]
           }
       ]
      }

#. Build the custom EKS AMI containing the |SDK|::

      packer build xilinx-vt-al2-eks.yml

   After the packer build finishes, the new EKS AMI will be available in the AWS console.

      