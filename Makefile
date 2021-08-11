TARGET=parrot.rom
SRC=parrot_code.c
NOTUSED=parrot_BSS.bin parrot_DATA.bin

#adjust this to fit the result of "convert -coalesce"
RGBS=parrot0.rgb parrot1.rgb parrot2.rgb parrot3.rgb parrot4.rgb parrot5.rgb parrot6.rgb parrot7.rgb parrot8.rgb parrot9.rgb

DATA=parrot_data.c
GIF=parrot.gif

UTIL=mkdata
UTIL_SRC=mkdata.c

# -lmsxbios required
$(TARGET): $(SRC) $(DATA)
	zcc +msx -lmsxbios -subtype=rom -compiler=sccz80 -o $@ $(SRC) $(DATA) 

$(DATA): $(RGBS) $(UTIL)
	./$(UTIL) $(RGBS) > $(DATA)

$(UTIL): $(UTIL_SRC)
	$(CC) $(CFLAGS) $< -o $@

$(RGBS): $(GIF)
	convert $(GIF) -coalesce parrot%d.rgb

$(GIF):
	wget https://github.com/jmhobbs/cultofthepartyparrot.com/raw/main/parrots/$(GIF)

clean:
	rm -rf $(GIF) $(RGBS) $(DATA) $(UTIL) $(TARGET) $(NOTUSED)
