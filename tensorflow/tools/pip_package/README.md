TensorFlow

- The following changes were made to fix the problem of constant failing of the "**AMD ROCm -- Community CI Build — rocm CI build**" check. 

- The issue is occuring to all the contributors due to a "**minor typo**" in the symbol. 

- Few changes and fixes were made to improve the compatibility of the code with "**Windows**".

- Additonal **refractoring** to certain file for needed to support some steps involving in the creation of "**.whl**" file.

1) Initially this [PR #69345](https://github.com/tensorflow/tensorflow/pull/69345#commits-pushed-48c59ff) is supposed to be a solution for this issue [Issue #69284](https://github.com/tensorflow/tensorflow/issues/69284). The issue was resolved, and PR was raised. 

2) When the PR is going through all the standard checks, one of the checks from tensroflow - "**AMD ROCm -- Community CI Build — rocm CI build**" failed. 

Now, the goal was totaly shifted to fix this issue.

An error - "Target NOT Declared" surfaced when the build command that is executed through bazel was fired. The command - **bazel build //tensorflow/tools/pip_package:build_pip_package_py**.

3) There was an issue with the way the "**target - build_pip_package_py**", which is initialized to the "**name**" attribute in the **py_runtime()** symbol. 

4) After thorough checking, it was evident that the target has to be - build_pip_package, not build_pip_package_py. As there is no such file name in the directory.

5) To fix that, we have to set the environment variables properly.

6) The OS that is being used is "Windows". The whole project was not configured to handle the operations from windows. No enviroment variable flags were added for windows in the **.bazelrc** file. Now it is added. Please see the ``.bazelrc`` file

7) Now, the **standard interpreter path** for the attribute "**py_runtime_path**" attribute from the py_runtime symbol in the **WORKSPACE** file, has to be understood for smooth **bazel** execution.

8) The correct "format" of the interpreter path compatible for **Windows** with bazel, was set. But the error that was mentioned earlier, persisted.

9) When looked back into the build_pip_package.py file, It was clear that the file **did not have** the "**main()**" fucntion in it. But the **py_runtime symbol** in the **BUILD** file **mentioned it**. Which is a wrong interpretation. That was fixed following with another error.

10) The same py_runtime symbol again had an issue with the "deps" attribute this time. The **name** of the **other target** was incorrect. It  - **//tensorflow/tools/pip_package/utils:py_utils**. The "py_utils" file was nowhere in the directory. So, there is nowhere to fetch or to look at.

11) It is corrected and changed to "utils". Since there is a file - "**utils.py**" in the utils directory, which has the **py_library symbol** in it, and it has to point to this file. In the same ```utils.py```, the value assigned to the **name** attribute in in the py_library is corrected as well. It was actualy given as "py_utils". Changed it to "utils".

12) The "**bazel repository**" mentioned in the **BUILD** file in the "**load**" statement was **wrong**. It was give - "load("@rules_python//python:repositories.bzl", "py_binary")". which is not working. Changed it to the correct target - load("@rules_python//python:defs.bzl", "py_binary"), and it worked.

13) With all the above fixes, the build was successful.

14) Now, after this, there was further "refractor" step in the code that had a need to be taken care of.

15) In the build_pip_package.py file, reference to **setup.py** file was made.

16) From two of the many "**argparse**" arguments in the build_pip_package.py file, two arguments were set to "**required=True**".

17) They are "**--output-name**" and "**--project-name**". This is for creation of "**.whl**" file.

18) Now, to integrate this, the "**pipeline jobs**" in the "**jenkins pipeline**" have to be adjusted accordingly for both the "**build_nightly**" and "**build_release**" .JenkinsFile files.

19) The --project-name was given static, so did not disturb it. But added new snippet for **fetching** the **--output-name dynamically** once the "**Binary Distribution**" is done through ``python setup.py bdist_wheel`` command.

20) The setup.py file is responsible to create the ".whl" file.

21) Now, while doing this, again there was error with the setup.py file. Since the setup.py file is not configured for windows properly.

22) So, the issue that is about to being mentioned, is with Windows itself. There is a "**name casing**" condition in-built in windows with the folder creation. Since there is already a file named "BUILD", and the binary distribution process includes a **creation of the folder** with name "**build**", before the generation of the ".whl" file.

23) But since there is this "name casing" issue, it is treating both the "BUILD" and "build" as same.

24) The **name of BUILD file cannot be changed**. Because this is associated with "bazel". Doing so, can distur the configurations. So, a **class** called "**CustomBuild()**" was constructed in the **setup.py** which handles this by initialy detecting the "OS". If it is windows, then we will be **changing the folder "name" to "build_on_windows"**. Else, we will keep the name undisturbed. Which will be - "**build**".

25) This worked, and the ``.whl`` file creation was successful.

26) With this, everything was successful. The build and the mandatory creation step for ".whl" file.

27) One more obseravtion was made in the build_pip_package.py file, whic are the arguments that have "required=True". Just incase if there is an issue that surfaced suring the wheel creation or something else occured, and the "dynamic" output name is not fetched, a "**default value**" has to be there for the **".whl" creation** to go ahead. So, added the default values.

28) Regarding the **interpreter path** for **Windows**, it is important to mention that currently in the "**WORKSPACE**" file in the the path **/ci/official/wheel_test**, the interpreter path is set to "**C:/Python311/python.exe**". This has to be **changed** based on the **OS** that the developer is working on, the "**python version**" and the "**standard bazel format of mentioning the interpreter path**" to make the "build" successful, going forward. 

29) The path and the required Environment Variables must also be added accordingly.

30) A typo with the variable name in the Jenkins file that I have pushed was causing issue in creating the ".whl" file. The issue was - In the build_pip_package.py file, the format of the name given --output-name and --project-name. But I have written it incorrectly which was --output_name and --project_name. The underscore was causing an issue.

31) The ``environment`` **stage** which was previously set up in the **Jenins jobs** was again not supported for **Windows**. The necessary changes were made to make sure that jenkins is pointing to the right executable.

##### Couple of changes were made in the build_pip_package.py file to enhance the ".whl" file creation.

1) Some changes and configuration might be small and not at all times the change will be an end-to-end "package creation. In this kind of case where a simple fixing and refractors are done and "Not an entire new Python Scripts are written which might include C/C++ extensions or in some case Python Modules" as well, the **.whl** file creation would not have any "**headers**" or "**srcs**" or "**aot**". 

2) The "**build_pip_package.py**" file in its previous format is not handling to "**bypass the check**" for ``--headers``, ``--srcs`` and ``--aot``. The **Bazel-Generated Packaging**, strictly expects to pass all the 3 parameters along with the ``--output-name`` and ``--project-name``.

3) Therefore as I mentioned in previous points, a condition was added to bypass headers, srcs and aot when not passed in the **Bazel-Generated Packaging** in the Jenkins job which is how it is, currently. The change was made keeping the "**action=append**" parameter fro both the headers and the srcs file. 

4) The import statement in the build_pip_package.py file were also corrected. It would make understanding better since both build_pip_package.py and utils folder are in the same directory/leve, we could simply do ``from utils import utils as utils_module``. By doing this, I simply did ``utils_module.copy_file()`` and so on to use the functions that are imported from the setup.py file. To make this work, one should **initially set a New "Environment Variable" under "System Variables" with the "Variable" as "PYTHONPATH" and the "Value" will be the "ABSOLUTE PATH" upto the "PARENT DIRECTORY" of the setup.py file**.

Therefore, the value for the PYTHONPATH variable will be ``"C:/Users/your_username/tensorflow/tensorflow/tools/pip_package"`` for **Windows**. For **Linux**, the equivalent path would be ``"/home/your_username/tensorflow/tensorflow/tools/pip_package"``. For **Mac OS X**, the equivalent path would be ``/Users/your_username/tensorflow/tensorflow/tools/pip_package``.

5) An empty ``__init__.py`` file was added in the utils folder as the utils.py file in that folder has couple of functions that were being imported to the build_pip_package.py file. 

6) The argument name ``cwd`` was replaced with ``project_location``, providing a clearer and more explicit description of what the variable represents.

7) The ``env.get("HOMEPATH", "C:")`` which checks for the existence of HOMEPATH and only sets it to "C:" if it isn’t already defined. This is safer as it doesn't overwrite existing environment settings unless necessary.

8) Replaced with ``env["collaborator_build"]`` to ``"1"`` instead of ``True``, ensuring consistency in environment variable types (storing strings instead of boolean values).

9) The **path** to ``setup.py`` was "hardcoded" as ``tensorflow/tools/pip_package/setup.py``. Now, changed it in such a way that it constructs the "**path dynamically**" using ``project_location``, making the script more flexible and adaptable to changes in directory structure without requiring code changes.

10) Previously, a **try...finally block** was set to ensure cleanup of the temporary directory. It is changed to handle the ``temporary directory`` using a **context manager** ``with()`` statement, which automatically takes care of cleanup. This makes the code cleaner and reduces the risk of leaving temporary files or directories if exceptions occur.

11) The previous version does not explicitly handle the renaming or moving of the wheel file after creation. Now, it is updated to check the list of wheel files in the specified directory, rename them, and move them to a final output directory based on the **environment variable** ``WHEEL_OUTPUT_DIR``. This addition makes the script handle end-to-end processing of wheel files, making deployment and distribution easier and more automated.

The, ``WHEEL_OUTPUT_DIR`` is basically added for clarification and handling of build outputs within the workflow. It doesn't directly participate in the distribution of the wheel file. It just plays a critical role in defining where automated scripts or manual processes should look for the wheel file, effectively supporting the distribution process indirectly.