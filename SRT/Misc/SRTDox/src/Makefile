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
	@cp SRTbackground.jpg $(INTROOT)/bitmaps 
	@chmod 644  $(INTROOT)/bitmaps/SRTbackground.jpg
	@cp SRTlogo.jpg $(INTROOT)/bitmaps
	@chmod 644 $(INTROOT)/bitmaps/SRTlogo.jpg
	@echo "installation done!"
