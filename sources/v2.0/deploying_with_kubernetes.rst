
.. _deploying-with-kubernetes:

#########################
Deploying with Kubernetes
#########################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1


.. Note::
	 A new Kubernetes plugin is available for U30 cards. This new plugin no longer requires an exact version number when specifying U30 resource limits. This allows for compatibility of pod description files across all releases of the |SDK|. Kubernetes clusters using this plugin will continue to work seamlessly even after an upgrade of the |SDK|. **If using Kubernetes, to prevent service degradation, it is necessary to deploy this new plugin (version 1.1.0)** before upgrading to the latest version of the |SDK|. For more details, refer to the instructions below.


********
Overview
********

The Xilinx device plugin for Kubernetes is a daemonset deployed on a Kubernetes cluster which:

- Discovers the Xilinx cards inserted in each node of the cluster and exposes information about their properties
- Enables containers to access Xilinx card resources installed in the worker nodes of the Kubernetes cluster

Noting that each Alveo U30 card is made of 2 devices, plugin provides device level granularity, allowing access to either individual devices or a card. Therefore, care should be taken in the pod's description, under "limits" field, to ensure that requested resources match those defined in the plugin, i.e., device or card. Furthermore, to isolate pod's description from future upgrades, plugin implements both generic and exact naming in pod's resources. See :ref:`Installing the Xilinx device plugin for Kubernetes <dev_plugin>` for specific details.

The plugin is available on the ECR repository: https://gallery.ecr.aws/xilinx_dcg/k8s-device-plugin

The following instructions explain how to install the Xilinx device plugin for Kubernetes and how to deploy user pods to access Xilinx devices.

In addition, a comprehensive step-by-step guide on how to run containerized workloads on Amazon ECS and Amazon EKS can be found on the AWS Compute Blog: https://aws.amazon.com/blogs/compute/deep-dive-on-amazon-ec2-vt1-instances/


*************
Prerequisites
*************

The following instructions assume that your system satisfies the following requirements:

- A Kubernetes cluster, e.g., Amazon EKS, is created and the |SDK| is installed on all U30/|VT1| worker nodes
- The client is configured to access the cluster:

  + kubectl is installed and connected to the cluster
  
Additionally for AWS EKS:
  + The AWS CLI is installed and the access key is correctly configured
  + eksctl is installed
 

For information regarding Kubernetes installation refer to https://github.com/Xilinx/FPGA_as_a_Service/blob/master/k8s-device-plugin/full-tutorial.md. For additional information about Amazon EKS, refer to the Amazon EKS user guide: https://docs.aws.amazon.com/eks/latest/userguide/getting-started.html


.. _dev_plugin:

**************************************************
Installing the Xilinx device plugin for Kubernetes
**************************************************
  
The following steps require kubectl to be connected to your cluster. After the Xilinx device plugin for Kubernetes is installed, no additional configuration is needed when adding nodes to the cluster.

#. Remove the current device plugin daemonset::

	kubectl delete daemonset current-device-plugin-daemonset-name -n kube-system.

#. Create a file called xilinx-device-plugin.yml and paste the following content in it::

	apiVersion: apps/v1
	kind: DaemonSet
	metadata:
	  name: device-plugin-daemonset
	  namespace: kube-system
	spec:
	  selector:
	    matchLabels:
	      name: device-plugin
	  template:
	    metadata:
	      annotations:
	        scheduler.alpha.kubernetes.io/critical-pod: ""
	      labels:
	        name: device-plugin
	    spec:
	      tolerations:
	      - key: CriticalAddonsOnly
	        operator: Exists
	      containers:
	      - image: public.ecr.aws/xilinx_dcg/k8s-device-plugin:1.1.0
	        name: device-plugin
	        env:
	        - name: U30NameConvention
	          value: "{CommonName|ExactName}"
	        - name: U30AllocUnit
	          value: "{Card|Device}"
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

   In the above yaml file, the following 2 environmental variables define naming convention and access granularity:
   
   .. option:: U30NameConvention

   - Defines how the resource name in the pod-description yaml file should be interpreted by the plugin. Allowed values are ``ExactName`` and ``CommonName``.

   - If set to ``CommonName``, the resource string used in the pod-description file must be set to ``amd.com/ama_u30``. This allows setting resource limits without having to specify an exact U30 firmware version number. Using ``CommonName`` provides forward and backward compatibility of pod-description files with respect to all releases of the |SDK|. This is the recommended setting.

   - If set to ``ExactName``, the resource string used in the pod-description file must be set to ``amd.com/xilinx_u30_gen3x4_base_2-0``. This allows setting resource limits for cards flashed with this specific version of the U30 firmware. When ``ExactName`` is used, there is no guarantee of forward or backward compatibility of pod-description files with respect to future releases of the |SDK|.

   - If this variable is not specified or is set incorrectly, it will default to ``CommonName``.

   .. option:: U30AllocUnit

   - Defines the unit for the resource numeration set in the pod-description yaml file. Allowed values are ``Card`` and ``Device``.

   - If set to ``Card``, the resource is measured in number of cards.

   - If set to ``Device``, the resource is measured in number of devices.

   - If this variable is not specified or is set incorrectly, it will default to ``Card``.


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
	  amd.com/ama_u30:                             4
	  attachable-volumes-aws-ebs:                  39
	  cpu:                                         24
	  ephemeral-storage:                           104845292Ki
	  hugepages-1Gi:                               0
	  hugepages-2Mi:                               0
	  memory:                                      47284568Ki
	  pods:                                        1
	Allocatable:
	  amd.com/ama_u30:                             4
	  attachable-volumes-aws-ebs:                  39
	  cpu:                                         23870m
	  ephemeral-storage:                           95551679124
	  hugepages-1Gi:                               0
	  hugepages-2Mi:                               0
	  memory:                                      46752088Ki
	  pods:                                        1

   In this example, 2 Alveo U30 cards are available in the node. Furthermore, the granularity of the plugin was set to Device and naming convention to CommonName.


*******************
Deploying User Pods
*******************

#. Confirm U30AllocUnit type, i.e., Card or Device, by issuing the following::

	kubectl describe daemonset device-plugin-daemonset -n kube-system | grep U30AllocUnit

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
	        amd.com/ama_u30: 1
	    command: ["/bin/sh"]
	    args: ["-c", "while true; do echo hello; sleep 10;done"]

  In the above example, the pod has a limit of 1 resource of the ``amd.com/ama_u30`` kind. Variables set in the daemonset yaml file determine the effective result of this setting.

  - When the :option:`U30NameConvention` variable is set to ``CommonName`` in the daemonset yaml file, the ``amd.com/ama_u30`` string allows setting resource limits without having to specify an exact U30 firmware version number. This provides backward and forward compatibility of pod-description file.

  - The effective unit for the limit count is determined by the choice of :option:`U30AllocUnit` in the daemonset yaml file. The allocation unit can be Card or Device.

#. Create the pod using the pod-description yaml file::

	kubectl create -f <pod-description.yml>

#. Check the status of the created pod::

	kubectl get pod -o wide

   You will see a similar report when the pod is successfully deployed on the cluster::

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
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
