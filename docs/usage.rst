#####
Usage
#####

TOMCAT PCO Writer client usage
------------------------------

.. code-block:: python
   :linenos:

    #!/bin/env python
    # -*- coding: UTF-8 -*-

    """
    POC Camera writer test script template 

    Description:
    Instantiates a PcoWriter object, configures, and receives 20 frames.

    """
    from epics import caput, caget
    import sys
    import time
    import getpass
    from datetime import datetime
    import os
    import inspect
    from pco_rclient import PcoWriter

    def get_datetime_now():
        return datetime.now().strftime("%H%M%S")

    ##########################################
    #### CAMERA CONFIGURATION AND METHODS ####
    ##########################################

    # IOC COMMANDS
    COMMANDS = {
        "CAMERA":       ":CAMERA",
        "FILEFORMAT":   ":FILEFORMAT",
        "RECMODE":      ":RECMODE",
        "STOREMODE":    ":STOREMODE",
        "CLEARMEM":     ":CLEARMEM",
        "SET_PARAM":    ":SET_PARAM",
        "SAVESTOP":     ":SAVESTOP",
        "FTRANSFER":    ":FTRANSFER"
    }
    # combines the IOCNAME:CMD for a epics command (caput/caget)
    def get_caput_cmd(ioc_name, command):
        return str(ioc_name+command)
    # starts the camera transfer
    def start_cam_transfer(n_frames):
        caput(get_caput_cmd(ioc_name, COMMANDS["SAVESTOP"]), n_frames) # Sets the number of frames to transfer
        caput(get_caput_cmd(ioc_name, COMMANDS["CAMERA"]), 1) # Starts the camera
        time.sleep(1)
        caput(get_caput_cmd(ioc_name, COMMANDS["FTRANSFER"]), 1) # Starts the transfer
    # stops the camera transfer
    def stop_cam_transfer():
        caput(get_caput_cmd(ioc_name, COMMANDS["CAMERA"]), 0) # Stops the camera
    # configures the camera
    def config_cam_transfer():
        caput(get_caput_cmd(ioc_name, COMMANDS["CAMERA"]), 0)
        caput(get_caput_cmd(ioc_name, COMMANDS["FILEFORMAT"]), 2)
        caput(get_caput_cmd(ioc_name, COMMANDS["RECMODE"]), 0)
        caput(get_caput_cmd(ioc_name, COMMANDS["STOREMODE"]), 1)
        caput(get_caput_cmd(ioc_name, COMMANDS["CLEARMEM"]), 1)
        caput(get_caput_cmd(ioc_name, COMMANDS["SET_PARAM"]), 1)

    ###############################
    #### SCRIPT USER VARIABLES ####
    ###############################
    # number of frames
    nframes = 20
    # defines the current time for the uniqueness of the output file
    output_str = get_datetime_now()

    # user id
    user_id = int(getpass.getuser()[1:])

    # IOC's name
    ioc_name = 'X02DA-CCDCAM2'
    #ioc_name = 'X02DA-CCDCAM3'

    # Output file path
    outpath = "/sls/X02DA/data/e{}/Data10/pco_test/".format(user_id)

    if not os.path.isdir(outpath):
        os.makedirs(outpath)

    # configure the camera
    config_cam_transfer()

    ###########################
    #### PCO CLIENT OBJECT ####
    ###########################
    pco_controller = PcoWriter(connection_address="tcp://129.129.99.104:8080", 
                            user_id=user_id)


    # is_connected
    print("pco_controller.is_connected()... (after new object)", end="")
    is_connected = pco_controller.is_connected()
    if not is_connected:
        problems += 1
        print(' ⨯')
    else:
        print(' ✓')

    if pco_controller.is_running():
        pco_controller.stop()

    problems = 0
    ok_flag = True

    ##############################################
    #### TEST METHODS WITH THE RUNNING WRITER ####
    ##############################################
    # runs the writer for an unlimited number of frames
    nframes = 20
    # configure
    print ("pco_controller.configure...", end="")
    conf_dict = pco_controller.configure(output_file=os.path.join(
        outpath, 'test'+output_str+'.h5'),user_id=user_id,
        dataset_name="data", n_frames=nframes)

    # status = configured
    if pco_controller.get_status() is not 'configured':
        problems += 1
        ok_flag = False
    if ok_flag:
        print(' ✓')
    else:
        print(' ⨯')
        ok_flag = True



    # start
    print("pco_controller.start...", end="")
    pco_controller.start()
    if pco_controller.get_status() == 'receiving':
        print(' ✓')
    else:
        print(' ⨯')



    # is_running
    print("pco_controller.is_running()... (after start)", end="")
    is_running = pco_controller.is_running()
    if not is_running:
        problems += 1
        print(' ⨯')
    else:
        print(' ✓')

    # gets status
    print('pco_controller.status()... (after start)', end="")
    if pco_controller.get_status() not in ['receiving', 'writing']:
        problems += 1
        print("Problem with get_status() method while running...")
        print(' ⨯')
    else:
        print(' ✓')


    # start nframes transfer via EPICS IOC CAPUT
    start_cam_transfer(nframes)
    # wait for nframes
    print('pco_controller.wait...')
    pco_controller.wait()
    # Stop the camera transfer via EPICS IOC CAPUT
    stop_cam_transfer()

    print("pco_controller.get_statistics_last_run()... (after start/stop)", end="")
    statistics_dict = pco_controller.get_statistics_last_run()
    statistics_ref = {'first_frame_id': '2466', 'user_id': '0', 'n_written_frames': '20', 'n_lost_frames': '0', 'end_time': 'Fri Oct  2 16:38:09 2020\n', 'start_time': 'Fri Oct  2 16:34:51 2020\n', 'n_frames': '20', 'dataset_name': 'data', 'duration_sec': '198.19', 'writing_rate': '0.10091326504869065', 'output_file': '/home/hax_l/software/lib_cpp_h5_writer/tomcat/output/test163451.h5', 'status': 'finished', 'success': True}
    if statistics_dict['success'] == False and statistics_dict['status'] is 'unknown':
        problems += 1
        ok_flag=False
    else:
        for key in statistics_ref:
            value = statistics_dict.get(key, None)
            if value is None:
                problems += 1
                ok_flag=False
    if ok_flag:
        print(' ✓')
    else:
        print(' ⨯')
        ok_flag = True

    # gets status
    print('pco_controller.status()... (after start/stop)', end="")
    if pco_controller.get_status() not in ['finished', 'stopping']:
        problems += 1
        print(' ⨯')
    else:
        print(' ✓')

    # get_status_last_run
    print('pco_controller.get_status_last_run()... (after start/stop)', end="")
    if pco_controller.get_status_last_run() != 'finished':
        problems += 1
        # print("Problem with get_status_last_run() after start/stop...")
        print(' ⨯')
    else:
        print(' ✓')

    # if problems == 0 -> success
    sys.exit(problems)

.. note::
    If the output file exists, data will be appended to it:
        * Make sure to change the **dataset_name** variable. If a dataset with the same name already exists on the output file, the writer will exit. 
        * Append to files at your own risk, if by any reason the second acquisition fails, the output file will be corrupted and the previously existing data will not be accessible any more.
    If the **n_frames** is 0, the writer will not stop until the client command is issued.

