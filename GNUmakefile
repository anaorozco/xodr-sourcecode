

include ../../../GDALmake.opt

OBJ	=	ogrxodrdriver.o ogrxodrdatasource.o ogrxodrlayer.o

CPPFLAGS	:=	-I.. -I../..  $(CPPFLAGS)

default:	$(O_OBJ:.o=.$(OBJ_EXT))

clean:
	rm -f *.o $(O_OBJ)

$(O_OBJ):	ogr_xodr.h