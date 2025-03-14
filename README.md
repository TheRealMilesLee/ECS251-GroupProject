# ECS251-GroupProject

This project aims to perform a comparison of the performance between different
thread pool strategies. We will compare the FIFO, LIFO, TBB, OpenMP and OpenBlas approaches. We
will also compare the native and async/future approaches.

The workload we will use is matrix multiplication. We will use
the block IK approach to ensure maximum performance in the algorithm side.

## Dependencies
This project is built using clang. So make sure you have the latest clang installed
before starting to build this project.

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



