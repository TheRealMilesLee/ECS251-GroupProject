# ECS251-GroupProject

This project aims to perform a comparison of the performance between different
thread pool strategies. We will compare the FIFO, LIFO, TBB, OpenMP and OpenBlas approaches. We
will also compare the native and async/future approaches.

The workload we will use is matrix multiplication. We will use
the block IK approach to ensure maximum performance in the algorithm side.

## Dependencies
This project is built using clang. So make sure you have the latest clang installed
before starting to build this project. Also, this project would only works in Linux platform, it would not compile on Windows or macOS platform due to the restrictions of openBlas and openMP

## Installation
Use the following commands to clone this repository.

```bash
git clone git@github.com:TheRealMilesLee/ECS251-GroupProject.git
```
Then go into the folder and make sure you have the clang dependencies. Then run
this command to build this project:

```bash
make all
```
After compilation, this will generate a series of executable files.

## Usage
Before starting the benchmark, make sure you have the ```perf``` tool ready on
your system. You can start the benchmark by running the script:

```bash
chmod +x perfRecord-amd-server.sh perfRecord-amd.sh perfRecord-intel.sh
```
Then, based on your laptop platform, you can launch the test by running following
command

```bash
./perfRecord-your-platform.sh
```
The amd-server option is for the AMD Threadripper series CPU and the amd option is
for any other AMD Ryzen series CPU, the intel option is for interl core series CPU.
Keep in mind that, different platform might have different perf support list, so if the
script failed and said "invalid events", please run
```bash
perf list
```
to see all events that support on the current machine.

## Code Structure
Here is a ```tree``` results for this project:
```txt
.
├── DA
│   ├── AMD_laptop_1024.csv
│   ├── AMD_laptop_4096.csv
│   ├── AMD_server_1024.csv
│   ├── AMD_server_4096.csv
│   ├── analysis.ipynb
│   ├── Intel_1024.csv
│   └── Intel_4096.csv
├── LICENSE
├── makefile
├── matrix_mul_double.cpp
├── matrix_mul.cpp
├── parallel_matrix_mul_async.cpp
├── parallel_matrix_mul_blas.cpp
├── parallel_matrix_mul_fifo.cpp
├── parallel_matrix_mul_lifo.cpp
├── parallel_matrix_mul_openmp.cpp
├── parallel_matrix_mul_standard.cpp
├── parallel_matrix_mul_tbb.cpp
├── parallel_matrix_mul.h
├── perfRecord-amd-server.sh
├── perfRecord-amd.sh
├── perfRecord-intel.sh
├── README.md
├── Results_AMD_Laptop
│   ├── 1024
│   │   ├── perfStats_async_20250309140256.txt
│   │   ├── perfStats_double_20250309140245.txt
│   │   ├── perfStats_fifo_20250309140252.txt
│   │   ├── perfStats_lifo_20250309140254.txt
│   │   ├── perfStats_openBLAS_20250309140300.txt
│   │   ├── perfStats_openMP_20250309140304.txt
│   │   ├── perfStats_parallel_standard_20250309140250.txt
│   │   ├── perfStats_single_20250309140242.txt
│   │   └── perfStats_tbb_20250309140258.txt
│   └── 4096
│       ├── perfStats_async_20250309141100.txt
│       ├── perfStats_double_20250309140543.txt
│       ├── perfStats_fifo_20250309140957.txt
│       ├── perfStats_lifo_20250309141028.txt
│       ├── perfStats_openBLAS_20250309141202.txt
│       ├── perfStats_openMP_20250309141252.txt
│       ├── perfStats_parallel_standard_20250309140926.txt
│       ├── perfStats_single_20250309140307.txt
│       └── perfStats_tbb_20250309141130.txt
├── Results_AMD_Laptop.7z
├── Results_AMD_Server
│   ├── 1024
│   │   ├── perfStats_async_20250309141615.txt
│   │   ├── perfStats_double_20250309141607.txt
│   │   ├── perfStats_fifo_20250309141612.txt
│   │   ├── perfStats_lifo_20250309141614.txt
│   │   ├── perfStats_openBLAS_20250309141619.txt
│   │   ├── perfStats_openMP_20250309141621.txt
│   │   ├── perfStats_parallel_standard_20250309141611.txt
│   │   ├── perfStats_single_20250309141554.txt
│   │   └── perfStats_tbb_20250309141617.txt
│   └── 4096
│       ├── perfStats_async_20250309142055.txt
│       ├── perfStats_double_20250309141803.txt
│       ├── perfStats_fifo_20250309142027.txt
│       ├── perfStats_lifo_20250309142041.txt
│       ├── perfStats_openBLAS_20250309142121.txt
│       ├── perfStats_openMP_20250309142157.txt
│       ├── perfStats_parallel_standard_20250309142014.txt
│       ├── perfStats_single_20250309141624.txt
│       └── perfStats_tbb_20250309142108.txt
├── Results_AMD_Server.7z
├── Results_Intel
│   ├── 1024
│   │   ├── perfStats_async_20250310053048.txt
│   │   ├── perfStats_double_20250310053029.txt
│   │   ├── perfStats_fifo_20250310053043.txt
│   │   ├── perfStats_lifo_20250310053046.txt
│   │   ├── perfStats_openBLAS_20250310053053.txt
│   │   ├── perfStats_openMP_20250310053101.txt
│   │   ├── perfStats_parallel_standard_20250310053040.txt
│   │   ├── perfStats_single_20250310053018.txt
│   │   └── perfStats_tbb_20250310053051.txt
│   └── 4096
│       ├── perfStats_async_20250310054627.txt
│       ├── perfStats_double_20250310053540.txt
│       ├── perfStats_fifo_20250310054334.txt
│       ├── perfStats_lifo_20250310054500.txt
│       ├── perfStats_openBLAS_20250310054921.txt
│       ├── perfStats_openMP_20250310055130.txt
│       ├── perfStats_parallel_standard_20250310054210.txt
│       ├── perfStats_single_20250310053106.txt
│       └── perfStats_tbb_20250310054755.txt
└── Results_Intel.7z
```
The project is organized as follows:

- [DA Folder](DA) contains the csv and jupyter notebook file for the data analysis
- [Results_AMD_Laptop](Results_AMD_Laptop) contains the results gathered from AMD laptop testbench
- [Results_AMD_Server](Results_AMD_Server) contains the results gathered from AMD server testbench
- [Results_Intel](Results_Intel) contains the results gathered from Intel laptop testbench

All of our implementation were in the [header file](parallel_matrix_mul.h). The CPP file is only for the runner.

We use bash scripts to automated the test process, based on different platform, we have different scripts.
```bash
perfRecord-amd-server.sh # This one is for the Threadripper 7960X platform
perfRecord-amd.sh # This one is for R7-7735HS platform
perfRecord-intel.sh # This one is for i7-7700HQ platform
```
So please choose cooresponding script based on YOUR platform. Because ```perf``` would have different event in different CPUs, this might not always work. We only can promise it would work on those three specific CPUs.
## Credit
This project was done by Hengyi Li, Zhuosheng Liu, Michael Gunnings and Jason Yoo
with the help of GitHub Copilot and ChatGPT for debugging. We would also like
to link the related libraries and handbooks we referenced down below.

## References
- [Intel TBB](https://www.intel.com/content/www/us/en/developer/tools/oneapi/onetbb.html)
- [OpenBLAS](http://www.openmathlib.org/OpenBLAS/)
- [OpenMP](https://www.openmp.org/)
- [Microsoft PPL](https://learn.microsoft.com/en-us/cpp/parallel/concrt/parallel-patterns-library-ppl?view=msvc-170)
- [C++ Reference](https://en.cppreference.com/w/)
- [GitHub Copilot](https://github.com/features/copilot)
- [ChatGPT](https://www.openai.com/research/chatgpt)



