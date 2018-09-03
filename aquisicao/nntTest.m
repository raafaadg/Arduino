clear all;close all;clc
format long

fileID = fopen('aquisicao13.txt','r');
y = fscanf(fileID,'%d,');
fclose(fileID);

fileID = fopen('aquisicao6.txt','r');
yy = fscanf(fileID,'%d');
fclose(fileID);

y = y-min(y);
y=y/max(y);
fat = max(y) * 0.05;

input=zeros(4,length(y));
table=zeros(2,length(y));

for j=1:length(y)
    if(j>1 && j<length(y))
        maiorMenos = logical(y(j)>y(j-1));
        maiorMais = logical(y(j)>y(j+1));
        menorMenos = logical(y(j)<y(j-1));
        menorMais = logical(y(j)<y(j+1));
        maiorFat = logical(y(j)>=fat);
        menorFat = logical(y(j)<fat);
        igualMenos = logical(y(j)==y(j-1));
        igualMais = logical(y(j)==y(j+1));
    else
        maiorMenos = false;
        maiorMais = false;
        menorMenos = false;
        menorMais = false;
        maiorFat = false;
        menorFat = false;
        igualMenos = false;
        igualMais = false;       
    end
   if y(j)==y(1)   % primeiro valor 
       input(:,j) = [0; 1; 2; y(j)];
       table(:,j) = [1 1];
   elseif y(j)==y(length(y))   % último valor
       input(:,j) = [0; 1; 2; y(j)];
       table(:,j) = [1 1];      


   elseif (maiorMenos && menorMais && maiorFat)   % subindo > fat
       input(:,j) = [0; 1; 1; y(j)];
       table(:,j) = [1 1];
   elseif (maiorMenos && igualMais && maiorFat)   % subindo estável > fat
       input(:,j) = [0; 1; 1; y(j)];
       table(:,j) = [2 1];
   elseif (igualMenos && menorMais && maiorFat)   % estável > fat
       input(:,j) = [0; 1; 1; y(j)];
       table(:,j) = [3 1];
   elseif (igualMenos && maiorMais && maiorFat)   % estável descendo > fat
       input(:,j) = [0; 1; 1; y(j)];
       table(:,j) = [4 1];
   elseif (menorMenos && maiorMais && maiorFat)   % descendo > fat
       input(:,j) = [0; 1; 1; y(j)];
       table(:,j) = [5 1];


   elseif (maiorMenos && menorMais && menorFat)   % subindo < fat
       input(:,j) = [0; 1; 0; y(j)];
       table(:,j) = [1 0];
   elseif (maiorMenos && igualMais && menorFat)   % subindo estável < fat
       input(:,j) = [0; 1; 0; y(j)];
       table(:,j) = [2 0];
   elseif (igualMenos && menorMais && menorFat)   % estável < fat
       input(:,j) = [0; 1; 0; y(j)];
       table(:,j) = [3 0];
   elseif (igualMenos && maiorMais && menorFat)   % estável descendo < fat
       input(:,j) = [0; 1; 0; y(j)];
       table(:,j) = [4 0];
   elseif (menorMenos && maiorMais && menorFat)   % descendo < fat
       input(:,j) = [0; 1; 0; y(j)];
       table(:,j) = [5 0];      
   end
end
% input = [zeros(size(y')); ones(size(y')); y'];
% table = zeros(size(y'));
% table(y>fat)=1;


yy = yy-min(yy);
yy=yy/max(yy);
% test = [zeros(size(yy')); ones(size(yy')); yy'];
test=zeros(4,length(yy));

for j=1:length(yy)
    if(j>1 && j<length(yy))
        maiorMenos = logical(yy(j)>yy(j-1));
        maiorMais = logical(yy(j)>yy(j+1));
        menorMenos = logical(yy(j)<yy(j-1));
        menorMais = logical(yy(j)<yy(j+1));
        maiorFat = logical(yy(j)>=fat);
        menorFat = logical(yy(j)<fat);
        igualMenos = logical(yy(j)==yy(j-1));
        igualMais = logical(yy(j)==yy(j+1));
    else
        maiorMenos = false;
        maiorMais = false;
        menorMenos = false;
        menorMais = false;
        maiorFat = false;
        menorFat = false;
        igualMenos = false;
        igualMais = false;       
    end
   if yy(j)==yy(1)   % primeiro valor 
       test(:,j) = [0; 1; 2; yy(j)];
   elseif yy(j)==yy(length(yy))   % último valor
       test(:,j) = [0; 1; 2; yy(j)];


   elseif (maiorMenos && menorMais && maiorFat)   % subindo > fat
       test(:,j) = [0; 1; 1; yy(j)];
   elseif (maiorMenos && igualMais && maiorFat)   % subindo estável > fat
       test(:,j) = [0; 1; 1; yy(j)];
   elseif (igualMenos && menorMais && maiorFat)   % estável > fat
       test(:,j) = [0; 1; 1; yy(j)];
   elseif (igualMenos && maiorMais && maiorFat)   % estável descendo > fat
       test(:,j) = [0; 1; 1; yy(j)];
   elseif (menorMenos && maiorMais && maiorFat)   % descendo > fat
       test(:,j) = [0; 1; 1; yy(j)];


   elseif (maiorMenos && menorMais && menorFat)   % subindo < fat
       test(:,j) = [0; 1; 0; yy(j)];
   elseif (maiorMenos && igualMais && menorFat)   % subindo estável < fat
       test(:,j) = [0; 1; 0; yy(j)];
   elseif (igualMenos && menorMais && menorFat)   % estável < fat
       test(:,j) = [0; 1; 0; yy(j)];
   elseif (igualMenos && maiorMais && menorFat)   % estável descendo < fat
       test(:,j) = [0; 1; 0; yy(j)];
   elseif (menorMenos && maiorMais && menorFat)   % descendo < fat
       test(:,j) = [0; 1; 0; yy(j)];
   end
end

x = input;
t = table;
trainFcn = 'trainscg';                        

hiddenLayerSize = 100;                          
net = patternnet(hiddenLayerSize);              

net.divideParam.trainRatio = 70/100;
net.divideParam.valRatio = 15/100;
net.divideParam.testRatio = 15/100;
net.performFcn = 'crossentropy';
plot(yy)

[net,tr] = train(net,x,t);
aux = net(test);

for i=1:length(aux)
    if(round(aux(i))>0.5)
        aux2(i) = yy(i);
    else
        aux2(i) = 0;
    end
end
figure
plot(aux2)