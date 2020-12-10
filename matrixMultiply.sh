#!/bin/bash
#$ -N Ajob
#$ -cwd
#$ -q UI-GPU
#$ -l gpu=true
#$ -l ngpus=1
/bin/echo Running shell script
module load cuda
nvcc matrixMultiply.cu -o run
./run
echo Done running shell script
