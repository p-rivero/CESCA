# Make sure that there's at least 1 argument
if [ -z "$1" ]
    then echo "Usage: ./send.sh file.asm\nfile.asm gets preprocessed, assembled and sent to the Arduino programmer"
    exit 1
fi

# Name of temporary file
temporary="temp.preprocessed"

# Preprocess and send file indicated in the argument
cpp -x assembler-with-cpp -nostdinc -CC -undef -P $1 > $temporary
./RAM_Programmer.exe $temporary
rm -f $temporary
