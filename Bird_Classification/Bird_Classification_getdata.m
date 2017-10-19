clc
clear 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% 학습데이터 로딩 및 전처리

path_name = 'E:\Projects\Matlab\vb100_audio\train';
lists = dir(path_name);
file_n = length(lists);

for i=1:length(lists) - 2
    file_name = strcat(path_name,'\',lists(i+2).name);
    [y,Fs] = audioread(file_name);
       
    [train_row,train_col] = size(y);
    
    if(train_col == 2)
        y = (y(:,1) + y(:,2))/2;
    end
    
    Tw = 25;           % analysis frame duration (ms)
    Ts = 10;           % analysis frame shift (ms)
    alpha = 0.97;      % preemphasis coefficient
    R = [ 20 20000 ];  % frequency range to consider
    M = 20;            % number of filterbank channels 
    C = 13;            % number of cepstral coefficients
    L = 22;            % cepstral sine lifter parameter

    hamming = @(N)(0.54-0.46*cos(2*pi*[0:N-1].'/(N-1)));
    MFCC_train(i) = struct('MFCCs','','FBEs','','frames','');

    [ MFCC_train(i).MFCCs, MFCC_train(i).FBEs,MFCC_train(i).frames ] = mfcc( y, Fs, Tw, Ts, alpha, hamming, R, M, C, L );
    
    [train_row,train_col] = size(MFCC_train(i).MFCCs);
       
    MFCC_train(i).MFCCs(isnan(MFCC_train(i).MFCCs)) = 0;

    for j = 1:7000
        
        if( j <= train_col)
            %z_score standardization
            MFCC_train(i).MFCCs(1:train_row,j) = (MFCC_train(i).MFCCs(1:train_row,j) - mean(MFCC_train(i).MFCCs(1:train_row,j))) / std(MFCC_train(i).MFCCs(1:train_row,j));
            mfcc_Train(i,j) = mean(MFCC_train(i).MFCCs(1:train_row,j));
            %Min_Max noamlliztion
            %MFCC_train(i).MFCCs(1:train_row,j) = (MFCC_train(i).MFCCs(1:train_row,j) - min(MFCC_train(i).MFCCs(1:train_row,j))) / (max(MFCC_train(i).MFCCs(1:train_row,j))) - min((MFCC_train(i).MFCCs(1:train_row,j)));
            %mfcc_Train(i,j) = mean(MFCC_train(i).MFCCs(1:train_row,j));
        else
            mfcc_Train(i,j) = 0;
        end
    end
end

disp('Get train data');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% 학습데이터를 기반으로한 KNN모델 구축

[num,train_Species] = xlsread('train_species.xlsx');
[num,test_Species] = xlsread('test_species.xlsx');

mdl = fitcknn(mfcc_Train,train_Species,'NumNeighbors',4);

disp('Model Construct');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%% 테스트데이터 로딩 및 전처리

path_name = 'E:\Projects\Matlab\vb100_audio\test';
lists = dir(path_name);
file_n = length(lists);

for i=1:length(lists) - 2
    file_name = strcat(path_name,'\',lists(i+2).name);
    [y,Fs] = audioread(file_name);
       
    [test_row,test_col] = size(y);
    
    if(test_col == 2)
        y = (y(:,1) + y(:,2))/2;
    end
    
    Tw = 25;           % analysis frame duration (ms)
    Ts = 10;           % analysis frame shift (ms)
    alpha = 0.97;      % preemphasis coefficient
    R = [ 20 20000 ];  % frequency range to consider
    M = 20;            % number of filterbank channels 
    C = 13;            % number of cepstral coefficients
    L = 22;            % cepstral sine lifter parameter

    hamming = @(N)(0.54-0.46*cos(2*pi*[0:N-1].'/(N-1)));
    MFCC_test(i) = struct('MFCCs','','FBEs','','frames','');

    [ MFCC_test(i).MFCCs, MFCC_test(i).FBEs,MFCC_test(i).frames ] = mfcc( y, Fs, Tw, Ts, alpha, hamming, R, M, C, L );
    
    [test_row,test_col] = size(MFCC_test(i).MFCCs);
    
    MFCC_test(i).MFCCs(isnan( MFCC_test(i).MFCCs)) = 0;
    
    for j = 1:7000
       if( j <= test_col)
            %z_score standardization
            MFCC_test(i).MFCCs(1:test_row,j) = (MFCC_test(i).MFCCs(1:test_row,j) - mean(MFCC_test(i).MFCCs(1:test_row,j))) / std(MFCC_test(i).MFCCs(1:test_row,j));
            mfcc_Test(i,j) = mean(MFCC_test(i).MFCCs(1:test_row,j));
            %Min_Max noamlliztion
            %MFCC_test(i).MFCCs(1:test_row,j) = (MFCC_test(i).MFCCs(1:test_row,j) - min(MFCC_test(i).MFCCs(1:test_row,j))) / (max(MFCC_test(i).MFCCs(1:test_row,j)) - min(MFCC_test(i).MFCCs(1:test_row,j)));
            %mfcc_Test(i,j) = mean(MFCC_test(i).MFCCs(1:test_row,j));
        else
            mfcc_Test(i,j) = 0;
        end
    end
end

disp('Get test data');