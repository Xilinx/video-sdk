
.. _deploying-with-kubernetes:

#####################################################
Deploying with Kubernetes
#####################################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1

*****************************************************
Overview
*****************************************************

The Xilinx device plugin for Kubernetes is a daemonset deployed on a Kubernetes cluster which:

- Discovers the Xilinx cards inserted in each node of the cluster and expose information about the devices and their properties
- Enables containers to access Xilinx cards installed in the worker nodes of the Kubernetes cluster

When the plugin mounts a Xilinx card, all the devices located on that card become available to the container. Since there are two Xilinx devices per Alveo U30 card, the Xilinx device plugin will always mount the two devices of each Alveo U30 card. In the pod-description file, the "limits" field can used to control how many cards are mounted, but both devices of the card will always be mave available to the container. For instance, on a vt1.6xl instance, the “xbutil command” will show 4 devices but a description of the Kubernetes worker node will show 2 Xilinx cards. The limit can be set to 1 in the pod-description file, in which case the container will have access to 2 devices.

The plugin is available on the ECR repository: https://gallery.ecr.aws/xilinx_dcg/k8s-fpga-device-plugin:1.0.100

The following instructions explain how to install the Xilinx device plugin for Kubernetes and how to deploy user pods with access to Xilinx devices.

|

*****************************************************
Prerequisites
*****************************************************

The following instructions assume that your system satisfies the following requirements:

- An Amazon EKS cluster is created and the |SDK| is installed on all |VT1| worker nodes
- The client is configured to access the EKS cluster:

  + The AWS CLI is installed and the access key is correctly configured
  + eksctl is installed
  + kubectl is installed and connected to the EKS cluster of |VT1| nodes

For additional information about Amazon EKS, refer to the Amazon EKS user guide: https://docs.aws.amazon.com/eks/latest/userguide/getting-started.html

|

*****************************************************
Installing the Xilinx device plugin for Kubernetes
*****************************************************

The following steps require kubectl to be connected to your EKS cluster. After the Xilinx device plugin for Kubernetes is installed, no additional configuration is needed when adding nodes to the cluster.

#. Create a file called xilinx-device-plugin.yml and paste the following content in it::

	apiVersion: apps/v1
	kind: DaemonSet
	metadata:
	  name: fpga-device-plugin-daemonset
	  namespace: kube-system
	spec:
	  selector:
	    matchLabels:
	      name: xilinx-fpga-device-plugin
	  template:
	    metadata:
	      annotations:
	        scheduler.alpha.kubernetes.io/critical-pod: ""
	      labels:
	        name: xilinx-fpga-device-plugin
	    spec:
	      tolerations:
	      - key: CriticalAddonsOnly
	        operator: Exists
	      containers:
	      - image: public.ecr.aws/xilinx_dcg/k8s-fpga-device-plugin:1.0.100
	        name: xilinx-fpga-device-plugin
	        securityContext:
	          allowPrivilegeEscalation: false
	          capabilities:
	            drop: ["ALL"]
	        volumeMounts:
	          - name: device-plugin
	            mountPath: /var/lib/kubelet/device-plugins
	      volumes:
	        - name: device-plugin
	          hostPath:
	            path: /var/lib/kubelet/device-plugins

#. Deploy the Xilinx device plugin as a daemonset::

	# Apply the Xilinx device plugin
	kubectl apply -f ./xilinx-device-plugin.yml 

	# Check the status of daemonset:  
	kubectl get daemonset -n kube-system  

	# Check the status of device-plugin pod:  
	kubectl get pod -n kube-system  

#. List visible nodes and check Xilinx resources available::

	# Get node names
	kubectl get node

	# Check Xilinx resources available in specific worker node
	kubectl describe node <node-name>

   For each node, you will see a similar report::

	Name:               ip-192-168-58-12.ec2.internal
	Roles:              <none>
	......
	Capacity:
	  attachable-volumes-aws-ebs:                  39
	  cpu:                                         24
	  ephemeral-storage:                           104845292Ki
	  hugepages-1Gi:                               0
	  hugepages-2Mi:                               0
	  memory:                                      47284568Ki
	  pods:                                        15
	  xilinx.com/fpga-xilinx_u30_gen3x4_base_1-0:  2
	Allocatable:
	  attachable-volumes-aws-ebs:                  39
	  cpu:                                         23870m
	  ephemeral-storage:                           95551679124
	  hugepages-1Gi:                               0
	  hugepages-2Mi:                               0
	  memory:                                      46752088Ki
	  pods:                                        15
	  xilinx.com/fpga-xilinx_u30_gen3x4_base_1-0:  2

   In this example, 2 Alveo U30 cards are available in the node.

|

*****************************************************
Deploying User Pods
*****************************************************

.. public.ecr.aws/xilinx_dcg/U30-Video-Solution:AL2-V1.5-20210907

#. Paste and customize the content below in a pod-description yaml file::

	apiVersion: v1
	kind: Pod
	metadata:
	  name: <pod-name>                                     # Choose a unique test pod name
	spec:
	  containers:
	  - name: <container-name>                             # Choose a unique container name
	    image: <path-to-docker-image>                      # Path to the Docker image on ECR
	    resources:
	      limits:
	        xilinx.com/fpga-xilinx_u30_gen3x4_base_1-0: 1  # Number of Alveo U30 cards which should be mounted 
	    command: ["/bin/sh"]
	    args: ["-c", "while true; do echo hello; sleep 10;done"]

   NOTE: The "limits" field can be used to control how many Xilinx cards should made available to the container. The plugin mounts all the devices present on a card. In the case of Alveo U30 cards, a limit of 1 means that 2 devices are mounted.

#. Create the pod using the pod-description yaml file::

	kubectl create -f <pod-description.yml>

#. Check the status of the created pod::

	kubectl get pod -o wide

   You will a similar report when the pod is successfully deployed on the cluster::

	NAME         READY   STATUS    RESTARTS   AGE
	<pod-name>   1/1     Running   0          1m

   Note: If the pod gets stuck during the container creation process or is being evicted, use the command below to get detailed status information::

    kubectl describe pod <pod-name>    

#. Log into the pod once it is in the running state::

    kubectl exec -it <pod-name> -- /bin/bash

#. Inside the pod, source the environment and verify that the Xilinx devices are correctly mounted and visible::

	source /opt/xilinx/xrt/setup.sh
	xbutil examine


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
