set -e
# do compiling
./mips-linux-gnu-gcc -Werror -std=c99 -march=mips32 -mabi=32 -mno-explicit-relocs -mno-split-addresses -S -O0 $1.c -o $1.S &&

# modify assembly code
sed -i '/^.\.ident/d' $1.S
sed -i '/^.\.section/d' $1.S
sed -i '/^.\.previous/d' $1.S
sed -i '/^.\.nan/d' $1.S
sed -i '/^.\.module/d' $1.S
sed -i '/^.\.abicalls/d' $1.S
sed -i '/^.\.type/d' $1.S
sed -i '/^.\.size/d' $1.S
sed -i '/^.\.cpload/d' $1.S
sed -i '/^.\.cprestore/d' $1.S

# change the segment type of global data from .text to .data
perl -i.original -0777 -pe 's/\.text\n\t\.globl\tn/\.data\n\t\.globl\tn/igs' $1.S

# add .data seg to global array
echo "Note: please make sure all global variables are set"
echo "and .data seg is added to your global array (refer to lab1-handout for more information)!"

# run
echo "There are two modes to emulate in SPIM:"
echo "./spim -lab1-dev file $1.S"
echo "./spim -lab1-rel stdin stdout file $1.S"
