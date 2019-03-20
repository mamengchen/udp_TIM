INCLUDE=-I /home/mmc/workpress/jsoncpp
LIB_PATH=/home/mmc/workpress/TIM/comm/lib/libjsoncpp.a
.PHONY:all
all:test
test:test.cc
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH)
