all:
	@echo "nothing to do!"

install:
	@cp DoxyfileIDL $(INTROOT)/config
	@chmod 644 $(INTROOT)/config/DoxyfileIDL
	@cp DoxyfileCPP $(INTROOT)/config
	@chmod 644 $(INTROOT)/config/DoxyfileCPP
	@chmod u+x configHtml 
	@./configHtml
	@cp DocHeader.html.temp $(INTROOT)/templates/DocHeader.html
	@rm DocHeader.html.temp
	@chmod 644 $(INTROOT)/templates/DocHeader.html
	@cp DocFooter.html $(INTROOT)/templates
	@chmod 644 $(INTROOT)/templates/DocFooter.html
	@cp MEDbackground.jpg $(INTROOT)/bitmaps 
	@chmod 644  $(INTROOT)/bitmaps/MEDbackground.jpg
	@cp MEDlogo.jpg $(INTROOT)/bitmaps
	@chmod 644 $(INTROOT)/bitmaps/MEDlogo.jpg
	@echo "installation done!"
