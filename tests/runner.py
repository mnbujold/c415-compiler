"""
Author: James Osgood

Call with a list of integers representing .pal files to test those files
using the pal compiler program. Appends results to the tests.log file.

Example usage:
$> python runner.py 0 1 2
# Runs the 0.pal, 1.pal, and 2.pal files.
$> python runner.py -a
# Runs all the *.pal test files.

"""
# TODO: improve documentation!
# TODO: allow custom log file name/path through command line args!

from os import getcwd, listdir, path
import platform
import re
from argparse import ArgumentParser
from subprocess import check_output
from datetime import datetime
import json

# Handle OSs
if platform.system() == 'Windows':
    PAL_PATH = '../pal.exe'
else:
    PAL_PATH = '../pal'
TEST_PATH = '%d.pal'
ERR_TAG = '$ERR_DATA$'
TAG_LENGTH = len(ERR_TAG)
HEAD_BASE_LEN = 5
ERROR_REGEX = '\{[\d]+\}.*\n\{\nError\! [\d]+:[\d]+ \- .*\n.*\n[ ]*\^[ ]*\n\}'

def _run_tests(test_list, test_data, log_file):
    """
    Runs the compiler on tests in test_list and appends the results of the
    comparison between with test_data to log_file.
    """
    output_msg = '------------------------\n'\
                 '| DATE: %s\n'\
                 '| TIME: %s\n'\
                 '| TEST: %s\n'\
                 '------------------------\n'
    output_log = open(log_file, 'a+')

    for test_index in test_list:
        start_datetime = datetime.today()
        start_date = start_datetime.date().isoformat()
        start_time = start_datetime.time().isoformat()
        error_results = ''
        test_name = TEST_PATH % test_index

        # So the file is written to in the correct order
        with open(log_file, 'a+') as output_log:
            output_log.write(output_msg % (start_date, start_time, test_name))

        with open(log_file, 'a+') as output_log:
            print 'running test program ' + test_name
            file_str = check_output([PAL_PATH, test_name], stderr = output_log)
            error_results = _parse_errors(file_str, test_data[test_name])
            output_log.write(error_results)
            
def _parse_errors(file_listing, correct_errors):
    """
    Returns a string of error comparisons between the errors found in
    file_listing and the correct_errors.
    """
    # TODO: also check character number with caret
    file_errors = re.findall(ERROR_REGEX, file_listing)
    error_results = []
    
    for error_msg in file_errors:
        line_num1, line_num2, char_num, message = _get_error_info(error_msg)
        line_buf = ' '*(len(str(line_num1)) + 3)
        correct_error = _get_matching_error(correct_errors, line_num1)
        
        error_results.append('| %d: %s' % (line_num1, message))
        
        if correct_error:
            error_correct = True
            
            if line_num1 != line_num2:
                error_results.append(
                    '|%sMismatching line numbers: %d =/= %d'
                    % (line_buf, line_num1, line_num2)
                    )
                error_correct = False
            
            if char_num != correct_error['char_number']:
                error_results.append(
                    '|%sIncorrect character number: %d =/= %d'
                    % (line_buf, char_num, correct_errors['char_number'])
                    )
                error_correct = False
            
            for keyword in correct_error['checklist']:
                if keyword not in message:
                    error_results.append(
                        '|%sError message missing keyword: %s'
                        % (line_buf, keyword)
                        )
                    error_correct = False
            
            if error_correct:
                error_results.append('|%sError success!'% line_buf)
            else:
                error_results.append('|%s*ERROR FAILURE!'% line_buf)            
            correct_errors.remove(correct_error)
        else:
            error_results.append('|%s*UNEXPECTED ERROR!' % line_buf)
        
        error_results.append('|')
    
    for missing_error in correct_errors:
        line_num = missing_error['line_number']
        line_buf = ' '*(len(str(line_num)) + 3)
        error_results.append('| %d: %s' % (line_num, missing_error['message']))
        error_results.append('|%s*MISSING ERROR!\n|' % line_buf)
    
    return '\n'.join(error_results)

def _get_matching_error(correct_errors, line_num):
    """
    Returns the error in correct_errors which has a line_number equal to
    line_num, or None if such an error cannot be found.
    """
    for error in correct_errors:
        if error['line_number'] == line_num:
            return error
        
    return None

def _get_error_info(error_msg):
    """
    Returns the line number, the error line number, error character number, and
    the error message from error_msg.
    """
    line, b0, error, line_copy, caret, b1 = error_msg.split('\n')
    
    # '{line_num1}'
    b_end = line.index('}')
    line_num1 = int(line[1:b_end])
    
    # 'Error! line_num2:chars_num'
    colon = error.index(':')
    line_num2 = int(error[7:colon])
    nums_end = error.index(' ', colon+1)
    char_num = int(error[colon+1:nums_end])
    
    return line_num1, line_num2, char_num, error[nums_end+3:]
    
def _get_test_files_data(test_list, update_tests):
    """ Returns the dictionary of error data for test lists. """
    test_data = {}
    
    for test_index in test_list:
        test_name = TEST_PATH % test_index
        error_list = None
        head_diff = 0
        
        with open(test_name, 'r+') as test_file:
            print 'reading test program ' + test_name
            error_list = _get_error_list(test_file)
            head_diff = _update_test_header(test_file,
                                            test_name,
                                            error_list,
                                            update_tests)

        # Account for different comment header size
        for error_data in error_list:
            error_data['line_number'] += head_diff
        test_data[test_name] = error_list
        
    return test_data

def _update_test_header(test_file, test_name, error_list, update_tests):
    """
    Updates the pal comment header for test_file with error_list. Returns the
    difference in comment header sizes (new lines - original lines).
    """
    test_file.seek(0)
    orig_header_size = _count_header_size(test_file)
    head_diff = (HEAD_BASE_LEN + len(error_list)) - orig_header_size
    orig_lines = []
    line_num = 0
    
    test_file.seek(0)
    for line in test_file:
        line_num += 1
        if line_num > orig_header_size:
            orig_lines.append(line)

    test_file.seek(0)
    if update_tests:
        print 'updating test program ' + test_name
        header = '{\n'\
                 '    %s\n'\
                 '    %s\n'\
                 '    Errors:\n'\
                 '%s'\
                 '}\n'\
                 % (test_name,
                    '$DUMMY_SUMMARY$',
                    _get_header_errors(error_list, head_diff))
        
        
        test_file.write(header)
        test_file.writelines(orig_lines)
        test_file.truncate()
    
    return head_diff
    
def _get_header_errors(error_list, head_diff):
    """ Return the string of error messages derived from error_list. """
    error_messages = []
    
    for error_data in error_list:
        error_messages.append(
        '    %d:%d %s\n' % (error_data['line_number'] + head_diff,
                            error_data['char_number'],
                            error_data['message'])
                            )
    
    return ''.join(error_messages)
    
def _count_header_size(test_file):
    """ Returns the size (in number of lines) of the first comment. """
    reading_header = False
    found_header = False
    comment_size = 0

    for line in test_file:
        if not found_header and line.strip() == '{':
            found_header = True
            reading_header = True
        if reading_header:
            comment_size += 1
            
            if line.strip() == '}':
                return comment_size
    
    return comment_size
             
def _get_error_list(test_file):
    """ Returns the error dictionaries parsed from test_file. """
    line_num = 0
    error_list = []

    for line in test_file:
        line_num += 1
        error_data = _get_error_data(line, line_num)
        
        if error_data:
            error_list.append(
                {
                    'line_number' : line_num,
                    'char_number' : error_data[0],
                    'message'     : str(error_data[1]),
                    'checklist'   : [str(token) for token in error_data[2]]
                })
    return error_list
    
def _get_error_data(line, line_num):
    """
    Returns json encoded error data parsed from line, or None if no parsable
    data is found.
    """
    try:
        l_index = line.index(ERR_TAG) + TAG_LENGTH
        r_index = line.rindex(ERR_TAG)

        if l_index >= r_index:   # One tag
            return None
            
        try:
            return json.loads(line[l_index:r_index].strip())
        except ValueError:
            print '%d: "%s" not json encodable string' % (line, line_num)
        
    except ValueError:  # No tags
        return None

def _get_cmdline_args():
    """ Returns the namespace of all parsed command line arguments. """
    dscrp_txt = 'Runs .pal test programs and pipes the tests\' output to '\
                'the test.log file. Does not run duplicate tests. Runs '\
                'tests in numerical order.'
    help_name_txt = 'the name of a T.pal test program to run'
    help_all_txt = 'run all *.pal tests'
    help_update_txt = 'update the comment headers in the test files to be run'
    help_norun_txt = 'do not run any of the test files (useful for updating '\
                     'without running)'
    help_log_txt = 'the log file that receives output from the compiler '\
                   '(Default: tests.log)'
    
    parser = ArgumentParser(description = dscrp_txt)
    parser.add_argument('test_names',
                        nargs = '*',
                        type = int,
                        help = help_name_txt,
                        metavar = 'T')
    parser.add_argument('-a',
                        '--all_tests',
                        action = 'store_true',
                        help = help_all_txt)
    parser.add_argument('-u',
                        '--update_tests',
                        action = 'store_true',
                        help = help_update_txt)
    parser.add_argument('-n',
                        '--no_run',
                        action = 'store_true',
                        help = help_norun_txt)
    parser.add_argument('-l',
                        '--log_file',
                        default = 'tests.log',
                        type = str,
                        help = help_log_txt,
                        metavar = 'FILE')
    
    return parser.parse_args()

def _get_valid_indices():
    """ Returns the names of all pal files in the current directory. """
    return [int(test_file.rsplit('.pal')[0]) for test_file in listdir(getcwd())
            if test_file.endswith('.pal')]

def _valid_index(test_index, valid_indices):
    """ Returns whether test_index is in valid_indices. """
    if test_index not in valid_indices:
        print 'test index %d ignored since not a valid index' % test_index
        return False
    return True

if __name__ == '__main__':
    """ Main. """
    test_runner = _get_cmdline_args()
    test_indices = list(set(test_runner.test_names))
    test_indices.sort()
    valid_indices = _get_valid_indices()
    
    if not path.isfile(PAL_PATH):
        print 'cannot find pal compiler at ../'
        exit()
    
    if test_runner.all_tests:
        if test_indices:
            print 'disregarding given test names since running all tests'
        valid_indices.sort()
        test_indices = valid_indices
    else:
        test_indices = [test_index for test_index in test_indices
                        if _valid_index(test_index, valid_indices)]
    
    test_data = _get_test_files_data(test_indices, test_runner.update_tests)
    if not test_runner.no_run:
        _run_tests(test_indices, test_data, test_runner.log_file)
