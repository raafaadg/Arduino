clear;close all;clc

% fileID = fopen('aquicicao1.txt','r');
% x = fscanf(fileID,'%f');
% fclose(fileID);

fileID = fopen('teste4.txt','r');
x = fscanf(fileID,'%d,');
fclose(fileID);

plot(x)
