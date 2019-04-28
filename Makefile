NOMS=DANKOU_Mathis-SOURSOU_Adrien

run: clean eval
	./eval "1=>NON0"
	./eval "(1.0)+1"
	./eval "NON(0)+0"
	./eval "(1=>(NON(1+0).1))"

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