

#. The |SDK| examples can be found in the :file:`/opt/xilinx/examples/u30` folder of your system. If this folder is not present, first :ref:`make sure your package management client points to the remote package repository<package-feed-configuration>` for the |SDK|. Then install the :file:`xilinx-alveo-u30-examples` package:

   + Ubuntu
 
   ::
 
     sudo apt-get install xilinx-alveo-u30-example   
 
   + RHEL and Amazon Linux 2
 
   ::
 
     sudo yum install xilinx-alveo-u30-example   

#. Configure the environment to use the |SDK|::

    source /opt/xilinx/xcdr/setup.sh


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.