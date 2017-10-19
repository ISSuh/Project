[BirdClass,score,cost] = predict(mdl,mfcc_Test);

count = 0;
[row,col] = size(BirdClass);

for i = 1:row
    bird = BirdClass{i,1};
    species = train_Species{i,1};
    if(size(bird) == size(species))
        count = count + 1;
    end
end

Rate = (count/ row) * 100

