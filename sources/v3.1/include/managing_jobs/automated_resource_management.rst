.. _using-job-descriptions:

**********************************
Automated Resource Management 
**********************************

The |SDK| provides a mechanism to automatically determine how many instances of a given job can be submitted to the system and on which device(s) to dispatch each job instance. This mechanism relies on Job Descriptions files and a Job Slot Reservation tool which calculates the resources required for each job, determines on which device each job should be run and reserves the resources accordingly.

**NOTE**: A current limitation of this method is that only one job description file can be used at a time, and a job description file can only contain a single job description. The system will only manage multiple jobs if they match the same description. Simultaneously managing different job types is not supported currently. The jobs need to be stopped before a new job description can be loaded. 



.. include:: ./include/managing_jobs/job_description_files.rst


.. include:: ./include/managing_jobs/job_slot_reservation_tool.rst


.. include:: ./include/managing_jobs/launcher_examples.rst




..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.