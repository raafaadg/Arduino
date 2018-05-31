clear;close all;clc

% fileID = fopen('aquicicao1.txt','r');
% x = fscanf(fileID,'%f');
% fclose(fileID);

fileID = fopen('teste.txt','r');
xx = fscanf(fileID,'%d,');
fclose(fileID);

% plot(x)
%figure
%plot(xx)
