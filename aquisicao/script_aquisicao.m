clear all;close all;clc
format long

fileID = fopen('aquisicao11.txt','r');
y = fscanf(fileID,'%d,');
% y = fscanf(fileID,'%d');
fclose(fileID);
conv = (1/1024);
y=y-min(y)-29;
y = y*conv;
dt1 = datenum([2018,8,14,22,0,0]);
dt2 = datenum([2018,8,15,6,0,0]);
testeX = linspace(dt1,dt2,size(y,1));
testeX_date = datetime(testeX,'ConvertFrom','datenum','Format','HH:mm');
% testeX_date2 = datetime(testeX,'ConvertFrom','datenum','Format','yyyy-MM-dd HH:mm:ssss');

% j=1;
% for i=1:size(y,1)
%     if y(i) >= 10*conv
%         yy(j,1) = y(i,1);
%         xx(j,1)=testeX_date(1,i);
%         zz(j,:) = {testeX_date2(1,i),y(i,1)};
%         j=j+1;
%     end
% end
% zz=zz';

% formatSpec = 'x%sy%d,';
% fileID = fopen('dadosFiltradosz1.txt','w');
% % [nrows,ncols] = size(zz);
% % for row = 1:nrows
% %     fprintf(fileID,formatSpec,zz{row,:});
% % end
% fprintf(fileID,'%d,',yy);
% fclose(fileID);

figure
% plot(xx(:,1),yy(:,1))
plot(testeX_date,y, 'LineWidth', 1.5)
set(gca,'FontSize',25);
title('Aquisição Bruxismo 14/08/2018', 'FontSize', 45)
ylabel('Sinal EMG (V)', 'FontSize', 60)
xlabel('Horário da Aquisição', 'FontSize', 45)
ylim([0 0.18])