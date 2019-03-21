INCLUDE=-I /home/mmc/workpress/jsoncpp
LIB_PATH=/home/mmc/workpress/TIM/comm/lib/libjsoncpp.a

server_chat:server_chat.cc 
	g++ -o $@ $^  $(INCLUDE) $(LIB_PATH) -lpthread
.PHONY:clean
clean:
	rm server_chat
