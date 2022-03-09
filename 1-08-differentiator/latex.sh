#!/bin/bash

build/release/main $1 && pdflatex -interaction=nonstopmode -halt-on-error $1
