<<<<<<< HEAD
"""
Author: James Osgood

Call with a list of integers representing .pal files to test those files
using the pal compiler program. Appends results to the tests.log file.

Example usage:
$> python runner.py 0 1 2
# Runs the 0.pal, 1.pal, and 2.pal files.
$> python runner.py --all
# Runs all the *.pal test files.

"""

from os import getcwd, listdir
from argparse import ArgumentParser
from subprocess import call
from datetime import datetime

LOG_NAME = 'tests.log'
PAL_PATH = '../pal'

def _run_tests(test_list):
    # TODO: use 'with' syntax to open/close files
    # TODO: check if pal is compiled yet - print message if it isn't
    output_msg = '----------------------\n'\
                 'DATE: %s\n'\
                 'TIME: %s\n'\
                 'TEST: %d.pal\n'\
                 '----------------------\n'
    test_path = '%d.pal'
    output_log = open(LOG_NAME, 'a+')
    
    for test_index in test_list:
        start_datetime = datetime.today()
        start_date = start_datetime.date().isoformat()
        start_time = start_datetime.time().isoformat()
        
        output_log = open(LOG_NAME, 'a+')
        output_log.write(output_msg % (start_date, start_time, test_index))
        output_log.close() # so the file is written to in the correct order
        
        output_log = open(LOG_NAME, 'a+')        
        call([PAL_PATH, test_path % test_index], stdout = output_log)
        output_log.close()

def _get_cmdline_args():
    dscrp_txt = 'Runs .pal test programs and pipes the tests\' output to '\
                'the test.log file. Does not run duplicate tests.'
    help_name_txt = 'the name of a T.pal test program to run'
    help_all_txt = 'run all *.pal tests'
    
    parser = ArgumentParser(description = dscrp_txt)
    parser.add_argument('test_names',
                        metavar = 'T',
                        type = int,
                        nargs = '*',
                        help = help_name_txt)
    parser.add_argument('-a',
                        '--all_tests',
                        action = 'store_true',
                        help = help_all_txt)
    
    return parser
    
def _get_valid_indices():
    return [int(test_file.rsplit('.pal')[0]) for test_file in listdir(getcwd())
            if test_file.endswith('.pal')]
    
def _valid_index(test_index, valid_indices):
    if test_index not in valid_indices:
        print 'test index %d ignored since not a valid index' % test_index
        return False
    return True
    
if __name__ == '__main__':
    valid_indices = range(0, 10)
    test_runner = _get_cmdline_args().parse_args()
    test_indices = list(set(test_runner.test_names))
    valid_indices = _get_valid_indices()
    
    if test_runner.all_tests:
        if test_indices:
            print 'disregarding given test names since running all tests'
        test_indices = valid_indices
    else:
        test_indices = [test_index for test_index in test_indices
                        if _valid_index(test_index, valid_indices)]
    
    _run_tests(test_indices)
=======
"""
Author: James Osgood

Call with a list of integers representing .pal files to test those files
using the pal compiler program. Appends results to the tests.log file.

Example usage:
$> python runner.py 0 1 2
# Runs the 0.pal, 1.pal, and 2.pal files.
$> python runner.py --all
# Runs all the *.pal test files.

"""

from os import getcwd, listdir, path
import platform
from argparse import ArgumentParser
from subprocess import call
from datetime import datetime

LOG_NAME = 'tests.log'

if platform.system() == 'Windows':
    PAL_PATH = '../pal.exe'
else:
    PAL_PATH = '../pal'

def _run_tests(test_list):
    # TODO: use 'with' syntax to open/close files
    output_msg = '----------------------\n'\
                 'DATE: %s\n'\
                 'TIME: %s\n'\
                 'TEST: %d.pal\n'\
                 '----------------------\n'
    test_path = '%d.pal'
    output_log = open(LOG_NAME, 'a+')
    
    for test_index in test_list:
        start_datetime = datetime.today()
        start_date = start_datetime.date().isoformat()
        start_time = start_datetime.time().isoformat()
        
        output_log = open(LOG_NAME, 'a+')
        output_log.write(output_msg % (start_date, start_time, test_index))
        output_log.close() # so the file is written to in the correct order
        
        output_log = open(LOG_NAME, 'a+')
        test_name = test_path % test_index
        print 'running test program ' + test_name
        call([PAL_PATH, test_name], stdout = output_log)
        output_log.close()

def _get_cmdline_args():
    dscrp_txt = 'Runs .pal test programs and pipes the tests\' output to '\
                'the test.log file. Does not run duplicate tests. Order of '\
                'tests may not be preserved.'
    help_name_txt = 'the name of a T.pal test program to run'
    help_all_txt = 'run all *.pal tests'
    
    parser = ArgumentParser(description = dscrp_txt)
    parser.add_argument('test_names',
                        metavar = 'T',
                        type = int,
                        nargs = '*',
                        help = help_name_txt)
    parser.add_argument('-a',
                        '--all_tests',
                        action = 'store_true',
                        help = help_all_txt)
    
    return parser

def _get_valid_indices():
    return [int(test_file.rsplit('.pal')[0]) for test_file in listdir(getcwd())
            if test_file.endswith('.pal')]

def _valid_index(test_index, valid_indices):
    if test_index not in valid_indices:
        print 'test index %d ignored since not a valid index' % test_index
        return False
    return True

if __name__ == '__main__':
    valid_indices = range(0, 10)
    test_runner = _get_cmdline_args().parse_args()
    test_indices = list(set(test_runner.test_names))
    valid_indices = _get_valid_indices()
    
    if not path.isfile(PAL_PATH):
        print 'cannot find pal compiler at ../'
        exit()
    
    if test_runner.all_tests:
        if test_indices:
            print 'disregarding given test names since running all tests'
        test_indices = valid_indices
    else:
        test_indices = [test_index for test_index in test_indices
                        if _valid_index(test_index, valid_indices)]
    
    _run_tests(test_indices)
>>>>>>> 728cca984355fed9371eae7bbad633e25ac9e37a
