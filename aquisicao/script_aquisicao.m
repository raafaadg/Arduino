clear;close all;clc

% fileID = fopen('aquicicao1.txt','r');
% x = fscanf(fileID,'%f');
% fclose(fileID);

fileID = fopen('aquisicao12.txt','r');
x = fscanf(fileID,'%d,');
x=x-30;
for k=1:size(x,1)
    if x(k) < 0
        x(k) = 0;
    end
end
fclose(fileID);

plot(x)

title('Aquisição 14/08/2018')
ylabel('Sinal 0-1 Volts convertido em 10bits)')
xlabel('Número de Amostras')