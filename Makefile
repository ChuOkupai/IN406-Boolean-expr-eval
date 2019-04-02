NOMS=DANKOU_Mathis-SOURSOU_Adrien

test: clean eval
	./eval BLABLA

clean:
	rm -f eval

eval: eval.c
	gcc -Wall -Werror -Wextra -Ofast $< -o $@

zip: cr.tex cr.pdf eval.c
	rm -rf $(NOMS) $(NOMS).zip
	mkdir $(NOMS)
	cp $^ Makefile $(NOMS)
	zip -9 -r $(NOMS).zip $(NOMS)
	rm -r $(NOMS)