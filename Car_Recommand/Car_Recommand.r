library("C50", lib.loc="~/R/win-library/3.2")
library("e1071", lib.loc="~/R/win-library/3.2")
library("stats", lib.loc="C:/Program Files/R/R-3.2.2/library")
library("e1071", lib.loc="~/R/win-library/3.2")
library("C50", lib.loc="~/R/win-library/3.2")
library("gmodels", lib.loc="~/R/win-library/3.2")
library("kernlab", lib.loc="~/R/win-library/3.2")
library("stats", lib.loc="C:/Program Files/R/R-3.2.2/library")
library("rpart", lib.loc="C:/Program Files/R/R-3.2.2/library")
library("rpart.plot", lib.loc="~/R/win-library/3.2")
library("gmodels", lib.loc="~/R/win-library/3.2") // 알고리즘과 여러 함수를 사용하기 위한 라이브러리 추가

car <- read.csv('Car.csv', stringsAsFactors = TRUE) // 데이터를 불러온다

car_train <- car[1:3000,] // 모델을 만들기 위해 학습할 데이터 약 3000개
car_test <- car[3001:4654,] // 모델을 테스트 하기 위한 데이터 약 1654개
car_int <- car 
car_int$choice <- as.integer(car_int$choice)  // 여러 알고리즘에 적용하기 위해 fator가 아닌 정수형으로도 데이터셋을 만들어둔다.
car_int_train <- car_int[1:3000,] // 정수형 choice인 데이터. 모델을 만들기 위해 학습할 데이터 약 3000개
car_int_test <- car_int[3001:4654,] // 정수형 choice인 데이터. 모델을 테스트 하기 위한 데이터 약 1654개
normal <- function(x) { return ((x - min(x)) / (max(x) - min(x))) } // 정규화가 필요한 알고리즘을 위해 정규화 함수를 만들어 둔다
car_nor <- as.data.frame(lapply(car_int,normal)) //미리 정규화를 한 데이터프레임을 만들어 둔다.

car_Naive_m <- naiveBayes(car_train,car_train$choice) // 나이브 베이즈 모델을 만든다
car_Naive_p <- predict(car_Naive_m,car_test) // 만든 나이브베이즈 모델에 테스트 데이터를 적용시킨다.
CrossTable(car_test$choice, car_Naive_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
View(car)

car_DST_m <- C5.0(car_train[-2],car_train$choice ) // 의사결정트리 알고리즘중 하나인 c5.0을 이용하여 모델을 만든다.
car_DST_m
summary(car_DST_m)
car_DST_p <- predict(car_DST_m,car_test) // 만든 의사결정트리 모델에 테스트 데이터를 적용시킨다.
CrossTable(car_test$choice, car_DST_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))

car_CART_m <- rpart(choice ~ .,data  = car_int_train) // 회귀트리 알고리즘인 CART를 이용하여 모델을 만든다.
car_CART_p <- predict(car_CART_m,car_int_test) // 만든 모델에 테스트 데이터를 적용시킨다.
summary(car_CART_p)
rpart.plot(car_CART_m,digits = 2) // 만든 모델의 회귀 트리 모형 그래프를 그린다.
cor(car_CART_p,car_int_test$choice) // 테스트 결과와 원래의 데이터의 관련성을 나타내는 상관계수를 구한다.
summary(car_CART_m)

car_Naive2_m <- naiveBayes(car_train,car_train$choice,laplace = 1) // 나이브베이즈 알고리즘에 라플라스 측정기의 값을 1로 하여 모델 생성한다.
car_Naive2_p <- predict(car_Naive2_m,car_test) // 생성된 모델에 테스트 데이터를 적용시킨다.
CrossTable(car_test$choice, car_Naive2_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
CrossTable(car_test$choice, car_Naive_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
car_Naive2_m <- naiveBayes(car_train,car_train$choice,laplace = 2)  // 나이브베이즈 알고리즘에 라플라스 측정기의 값을 2로 하여 모델 생성한다.
car_Naive2_p <- predict(car_Naive2_m,car_test) // 생성된 모델에 테스트 데이터를 적용시킨다.
CrossTable(car_test$choice, car_Naive_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
CrossTable(car_test$choice, car_Naive2_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))