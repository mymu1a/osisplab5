build: parent

parent: main.cc circleQueue.cc produser.cc consumer.cc
	gcc -o parent $^
