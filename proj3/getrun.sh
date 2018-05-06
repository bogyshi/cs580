# Script for interactive session (--gres=gpu:1 made it work)
srun --partition=gpucompute --gres=gpu:1 --pty bash

# Modules to include
module add cuda91/toolkit/9.1.85

