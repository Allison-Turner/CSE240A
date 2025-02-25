# Branch Prediction Algorithm Simulator
By Allison Turner & James Yuan

For CSE 240A Fall 2021

## Algorithms
### gshare

### tournament

### Perceptron

## Dependencies

## Makefile struggles
```
CC=gcc
OPTS=-fPIE -g -std=gnu99 -Werror

all: main.o predictor.o python3 pip
	$(CC) $(OPTS) -lm -o predictor main.o predictor.o
	pip3 install -r requirements.txt

main.o: main.c predictor.h
	$(CC) $(OPTS) -c main.c

predictor.o: predictor.h predictor.c
	$(CC) $(OPTS) -c predictor.c

python3:
	@if [ -z $(which python3)]; then\
		bash -c "apt-get -y update";\
		bash -c "apt install -y software-properties-common";\
		bash -c "add-apt-repository -y ppa:deadsnakes/ppa";\
		bash -c "apt-get -y update";\
		bash -c "apt-get install -y python3.8";\
	fi;\
	alias python3=python3.8;

pip: python3
	@if [ -z $(which pip3)]; then\
		bash -c "apt-get install -y python3-pip";\
	fi;

clean:
	rm -f *.o predictor
	rm -rf __pycache__
```