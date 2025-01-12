# ECS251-GroupProject

This project is aims to create an easy to use multithreading tool for the
common application in the bash shell.

We all know the shell only supports `&` symbol to make your tasks run in parallel,
but that's running on multi-process. We propose a way for you to run your task
in multi-thread, which is way faster than the multi-process approach and more
resource efficient.

## Dependencies
This project is built by clang. So make sure you have your latest clang installed'
before start to build this project

## Installation
Using the following commands to clone this repository.

```bash
git clone git@github.com:TheRealMilesLee/ECS251-GroupProject.git
```
Then go into the folder and make sure you have the clang dependencies. Then run
this command to build this project

```bash
make all
```
After the compile, you can move the executable to your bin directory

```bash
mv parallel_execute /usr/local/bin/
```
Lastly, add this to your path. If you're using bash, run this
``` bash
echo 'export PATH=$PATH:/usr/local/bin' >> ~/.bashrc
source ~/.bashrc
```
If you're using ZSH, run this
```zsh
echo 'export PATH=$PATH:/usr/local/bin' >> ~/.zshrc
source ~/.zshrc
```

## Usage

Run the `parallel_execute` tool using the following format:

```bash
parallel_execute <job name> <job parameters> <num_threads>
```

### Examples

1. **Matrix Multiplication**

```bash
parallel_execute matrix_multiply matrix1 matrix2 destination_matrix 8
```

2. **Sort**

```bash
parallel_execute array_sort src_array 8
```

3. **Array Sum**

```bash
parallel_execute array_sum src_array 8
```

4. **Array Search**

```bash
parallel_execute array_search src_array num_search 8
```
