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
library("gmodels", lib.loc="~/R/win-library/3.2") // �˰���� ���� �Լ��� ����ϱ� ���� ���̺귯�� �߰�

car <- read.csv('Car.csv', stringsAsFactors = TRUE) // �����͸� �ҷ��´�

car_train <- car[1:3000,] // ���� ����� ���� �н��� ������ �� 3000��
car_test <- car[3001:4654,] // ���� �׽�Ʈ �ϱ� ���� ������ �� 1654��
car_int <- car 
car_int$choice <- as.integer(car_int$choice)  // ���� �˰��� �����ϱ� ���� fator�� �ƴ� ���������ε� �����ͼ��� �����д�.
car_int_train <- car_int[1:3000,] // ������ choice�� ������. ���� ����� ���� �н��� ������ �� 3000��
car_int_test <- car_int[3001:4654,] // ������ choice�� ������. ���� �׽�Ʈ �ϱ� ���� ������ �� 1654��
normal <- function(x) { return ((x - min(x)) / (max(x) - min(x))) } // ����ȭ�� �ʿ��� �˰����� ���� ����ȭ �Լ��� ����� �д�
car_nor <- as.data.frame(lapply(car_int,normal)) //�̸� ����ȭ�� �� �������������� ����� �д�.

car_Naive_m <- naiveBayes(car_train,car_train$choice) // ���̺� ������ ���� �����
car_Naive_p <- predict(car_Naive_m,car_test) // ���� ���̺꺣���� �𵨿� �׽�Ʈ �����͸� �����Ų��.
CrossTable(car_test$choice, car_Naive_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
View(car)

car_DST_m <- C5.0(car_train[-2],car_train$choice ) // �ǻ����Ʈ�� �˰����� �ϳ��� c5.0�� �̿��Ͽ� ���� �����.
car_DST_m
summary(car_DST_m)
car_DST_p <- predict(car_DST_m,car_test) // ���� �ǻ����Ʈ�� �𵨿� �׽�Ʈ �����͸� �����Ų��.
CrossTable(car_test$choice, car_DST_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))

car_CART_m <- rpart(choice ~ .,data  = car_int_train) // ȸ��Ʈ�� �˰����� CART�� �̿��Ͽ� ���� �����.
car_CART_p <- predict(car_CART_m,car_int_test) // ���� �𵨿� �׽�Ʈ �����͸� �����Ų��.
summary(car_CART_p)
rpart.plot(car_CART_m,digits = 2) // ���� ���� ȸ�� Ʈ�� ���� �׷����� �׸���.
cor(car_CART_p,car_int_test$choice) // �׽�Ʈ ����� ������ �������� ���ü��� ��Ÿ���� �������� ���Ѵ�.
summary(car_CART_m)

car_Naive2_m <- naiveBayes(car_train,car_train$choice,laplace = 1) // ���̺꺣���� �˰��� ���ö� �������� ���� 1�� �Ͽ� �� �����Ѵ�.
car_Naive2_p <- predict(car_Naive2_m,car_test) // ������ �𵨿� �׽�Ʈ �����͸� �����Ų��.
CrossTable(car_test$choice, car_Naive2_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
CrossTable(car_test$choice, car_Naive_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
car_Naive2_m <- naiveBayes(car_train,car_train$choice,laplace = 2)  // ���̺꺣���� �˰��� ���ö� �������� ���� 2�� �Ͽ� �� �����Ѵ�.
car_Naive2_p <- predict(car_Naive2_m,car_test) // ������ �𵨿� �׽�Ʈ �����͸� �����Ų��.
CrossTable(car_test$choice, car_Naive_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))
CrossTable(car_test$choice, car_Naive2_p,prop.chisq = FALSE , prop.t = FALSE , dnn = c('actual', 'predicted'))