clear;close all;clc
fileID = fopen('equicicao1.txt','r');
x = fscanf(fileID,'%f');
fclose(fileID);
plot(x)