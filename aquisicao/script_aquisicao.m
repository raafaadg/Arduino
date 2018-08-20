clear;close all;clc
format long

% fileID = fopen('aquicicao1.txt','r');
% x = fscanf(fileID,'%f');
% fclose(fileID);

fileID = fopen('aquisicao13.txt','r');
x = fscanf(fileID,'%d,');
x=x-20;
for k=1:size(x,1)
    if x(k) < 0
        x(k) = 0;
    end
end
fclose(fileID);
time.mili = size(x,1)*100;
time.sec = time.mili/1000;
time.min = time.sec/60;
time.hora = time.min/60;

dt1 = datenum([2018,8,16,22,0,0]);
dt2 = datenum([2018,8,17,6,0,0]);
testeY = linspace(dt1,dt2,size(x,1));
testeY_date = datetime(testeY,'ConvertFrom','datenum');
plot(testeY_date,x, 'LineWidth', .3)

title('Aquisi��o Bruxismo 16/08/2018', 'FontSize', 60)
ylabel('Sinal 0-1 Volts convertido em 10bits', 'FontSize', 30)
xlabel('Hor�rio da Aquisi��o', 'FontSize', 45)