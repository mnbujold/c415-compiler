from argparse import ArgumentParser
from subprocess import call
from datetime import datetime

def _all_tests(test_list):
    print 'not implemented yet ...'

def _run_tests(test_list):
    output_msg = '----------------------\n'\
                 'DATE: %s\n'\
                 'TIME: %s\n'\
                 'TEST: %d.pal\n'\
                 '----------------------\n'
    run_cmds = ['./lex']
    test_path = 'tests/%d.pal'
    
    output_log = open('tests.log', 'a+')
    
    for test_index in test_list:
        test_file = open(test_path % test_index, 'r')
        start_datetime = datetime.today()
        start_date = start_datetime.date().isoformat()
        start_time = start_datetime.time().isoformat()
        
        output_log = open('tests.log', 'a+')
        output_log.write(output_msg % (start_date, start_time, test_index))
        output_log.close() # so the file is written to in the correct order
        
        output_log = open('tests.log', 'a+')
        call(run_cmds, stdin = test_file, stdout = output_log)
        test_file.close()
        output_log.close()

def _get_cmdline_args():
    dscrp_txt = 'run *.pal test programs in the tests/ directory'
    help_name_txt = 'a name of an [0-9].pal test'
    help_all_txt = 'run all *.pal tests (default: false)'
    
    parser = ArgumentParser(description = dscrp_txt)
    parser.add_argument('test_names',
                        metavar = 'T',
                        type = int,
                        nargs = '*',
                        help = help_name_txt)
    parser.add_argument('--all',
                        dest = 'do_tests',
                        const = _all_tests,
                        default = _run_tests,
                        nargs = '?',
                        help = help_all_txt)
    
    return parser
    
if __name__ == '__main__':
    test_runner = _get_cmdline_args().parse_args()
    
    test_runner.do_tests(test_runner.test_names)
