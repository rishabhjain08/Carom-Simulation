carrom:*.cpp
	@gcc carrom.cpp caromGUI.cpp physics.cpp Ball.cpp server.cpp client.cpp ai.cpp ai2.cpp text3d.cpp -lglui -lGLU -lglut -lpthread -o carrom
doc:
	@pdflatex carrom.tex
networked1:
	@sleep 2
	@./carrom j1-Client1 localhost
networked2:
	@sleep 4
	@./carrom j2-Client2 localhost
networked3:
	@sleep 6
	@./carrom j1-Client3 localhost
test1: carrom
	@./carrom c1 n c2 n
test2:
	@make -j test2p1 networked1
test2p1:
	@./carrom c2 n o n
test3:
	@make -j test3p1 networked1 networked2 networked3
test3p1:
	@./carrom c2 o o o
clean:
	@rm carrom a.out carrom.pdf carrom.bbl carrom.blg carrom.dvi carrom.ps carrom.log carrom.aux
