#!/usr/bin/python

# shuf.sh
# William Randall

# shuf:
# --input-range (-i)=LO-HI          treat each number LO through HI as an input line
# --head-count (-n)=COUNT           output at most COUNT lines
# --repeat (-r)                     output lines can be repeated
# --help                            display help and exit
# zero non-option arguments		    read from standard input
# a single non-option argument "-"  read from standard input

# if --repeat (-r) is used without --head-count (-n) the program should run forever

# report error if given invalid args

import random, sys, argparse, string

# class definitions

class RandLine:
    def __init__(self, filename):
        f = open(filename, 'r')
        self.l = []
        for l in f:
            self.l.append(l.rstrip()) 
        f.close()

    def shuf_lines(self):
        return random.sample(self.l, len(self.l))

# function definitions

def extraneous_args_check(exargs,parser):
    amt_exargs = len(exargs)
    if amt_exargs > 0:
        # check if invalid operand
        for i in range(0, amt_exargs):
            if exargs[i][0] == '-':
                parser.error("invalid option -- '{a}'".format(a=exargs[i][1]))
        # check extra operand
        parser.error("extra operand -- '{a}'".format(a=exargs[0]))

def i_option(args,parser):
    
    if args.input_range is None:
        return (-1,-1)

    if len(args.input_range) > 1:
        parser.error(" extra operand '{a}'".format(a=args.input_range[1]))

    if args.file_name is not None:
        parser.error(" extra operand '{a}'".format(a=args.file_name))

    i_range = args.input_range[0][0]
    lo_hi = i_range.split('-')
    
    if len(lo_hi) > 2:
        parser.error(" invalid input range: '{a}'".format(a=i_range))
   
    if len(lo_hi) != 2:
        parser.error(" invalid input range: '{a}'".format(a=i_range))
 
    lo=int(lo_hi[0])
    hi=int(lo_hi[1])
    
    if lo > hi or lo < 0 or hi < 0:
        parser.error(" invalid input range: '{a}'".format(a=i_range))
    
    return (lo,hi)

def n_option(args,parser):
    
    if args.head_count is None:
        return -1 
 
    if min(args.head_count)[0] < 0:
        parser.error("invalid line count: '{a}'".format(a=args.head_count[0][0]))
    
    else:
        return min(args.head_count)[0]

def make_rand_il(i_option, i_list, parser, args):
    input_list = []
    rand_il = []

    #input stream
    if i_option is True:
        input_list = i_list
        # randomize numbers in i_list
        rand_il = random.sample(input_list, len(input_list))
    else:
        # standard input
        if (args.file_name is None) or (args.file_name == '-'):
            try:
                for l in sys.stdin:
                    input_list.append(l.rstrip())
                # randomize stdio lines
                rand_il = random.sample(input_list, len(input_list))
            except KeyboardInterrupt as e:
                sys.exit("")

        # file io
        else:
            try:
                rl_object = RandLine(args.file_name)
                # randomize file lines
                rand_il = rl_object.shuf_lines()
            except:
                parser.error("{a}: No such file or directory".format(a=args.file_name))    

    # check if random list is empty
    if len(rand_il) == 0:
        parser.error("input is empty")
    
    return rand_il

def p_output(i_nums, i_list, n_num, parser, args):
    # check for options
    n_option = False
    if n_num != -1:
        n_option = True

    i_option = False
    if i_nums != (-1,-1):
        i_option = True

    rand_il = []
    rand_il = make_rand_il(i_option, i_list, parser, args) 
    
    index = 0
    limit = 0
    # n_option
    if n_option is True:
        # output till n_num for input_list
        limit = min(len(rand_il),n_num)
    else:
        limit = len(rand_il)
        # output till end of rand_il for rand_il

    while index < limit:
        sys.stdout.write(str(rand_il[index]) + "\n")
        index = index + 1
    return

def p_r_output(i_nums, i_list, n_num, parser, args):
    # check for options
    n_option = False
    if n_num != -1:
        n_option = True

    i_option = False
    if i_nums != (-1,-1):
        i_option = True

    rand_il = []
    rand_il = make_rand_il(i_option, i_list, parser, args)

    index = 0
    limit = 0
    # n_option
    if n_option is True:
        # output till n_num for input_list
        limit = n_num
        while index < limit:
            sys.stdout.write(str(random.choice(rand_il)) + "\n")
            index = index + 1
    else:
        # output till keyboard exit
        while True:
            try:
                sys.stdout.write(str(random.choice(rand_il)) + "\n")
            except KeyboardInterrupt as e:
                sys.exit("")
    return

# main function, i.e., starting point in Python
def main():
    usage_msg = """{name} [OPTION]... [FILE]
  or: {name} -i LO-HI [OPTION]...
Write a random permutation of the input lines to standard output.

With no FILE, or when FILE is -, read standard input.""".format(name = sys.argv[0])
   
    parser = argparse.ArgumentParser(usage = usage_msg)
 
    parser.add_argument(dest='file_name'
                        ,nargs='?'
                        ,help=argparse.SUPPRESS)

    parser.add_argument('-r'
                        ,'--repeat'
                        ,help='output lines can be repeated'
                        ,action='store_true'
                        ,default=False)

    parser.add_argument('-i'
                        ,'--input-range'
                        ,help='treat each number LO through HI as an input line'
                        ,action='append'
                        ,nargs = 1
                        ,metavar='')

    parser.add_argument('-n'
                        ,'--head-count'
                        ,help='output at most COUNT lines'
                        ,action='append'
                        ,type=int
                        ,nargs = 1
                        ,metavar='')
    
    args, other = parser.parse_known_args()

    extraneous_args_check(other,parser)

    i_nums = i_option(args,parser) 
    i_list = list(range(i_nums[0],i_nums[1] + 1))  

    n_num = n_option(args,parser)

#   print(args)

#   print("i: {0}".format(i_nums))
#   print("ilist: {0}".format(i_list))

#   print("n: {0}".format(n_num))

    if args.repeat is True:
        p_r_output(i_nums, i_list, n_num, parser, args)
    else:
        p_output(i_nums, i_list, n_num, parser, args)
    
if __name__ == '__main__':
    main()
