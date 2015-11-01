#!/bin/bash

mencoder "mf://output/*.bmp" -mf fps=30:type=bmp -ovc x264 -o $output

