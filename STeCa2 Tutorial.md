## Tutorial STeCa2 ##

**Gui overview**

####Menues
All hotkeys are noted in the menu next to the item if available.

| Items | Discription | 
|----------------|-----------------------------------------------------------|
| File           | Add Files to current session or load/save session |
| Edit           | Not jet implemented |
| View           | Display with/without fixed intensity scale for image and diffraction diagram. Show or hide image cut. Selection tools for the background and peak fitting. Also options to hide/display the Files / Datasets / Datasetsinfo panels, the status bar (displayed at the bottom edge) and view in full screen mode. |
| Datasets | Rotate image and mirror image. Load correction file.|
| Reflections | Add and remove reflections. |
| Output | Calculate pole figures and generate histograms. |
| Help | About STeCa |

####Icons

| icon | Description|
| ------| ---------------------------------------|
| 1.  | Add / remove files                     |
| 2. | Add correction file and enable / disable it.|
| 3. | Remove correction file |
| 4. | Enable or disable beam offset. This is used for x-ray instruments |
| 5. | Rotate image clock-wise by 90° |
| 6. | Mirror image |
| 7. | Display image with/without fixed intensity scale| 
| 8. | Set image cut for top, bottom, left, right.|
| 9. | Use the same value for all cuts |
| 10.| Show or hide image cut |
| 11.| Select regions for background fitting |
| 12.| Clear selection of background fitting |
| 13.| Display fitted background polynomial |
| 14.| Select area for peak fitting |
| 15.| Add and remove Reflections |

####Panels

#####Files panel and Datasets panel
In the files panel all loaded files are displayed. By selecting a file or multiple files the datasets panel will be populated with the collected datasets from the selected file/-s. Below the files panel the correction file has a separate display field. With the icon button 2. one can add a correction file which will be enabled by default. After a correction file is loaded the button will a low the user to enable / disable the correction file. Clearing the correction file is possible with icon 3. Now a new correction file can be loaded again.
Below the datasets panel it is possible so set the number of how many datasets should be combined. If the combination leaves a remainder then these will be added together in the last entry.

#####Image panel
The image panel contains two tabs, one for the data image and one for the correction image.

#####Fitting panel
The first tab is for the background fitting. With the background selection tool the regions for the background can be set in the diffraction diagram. When the mouse is moved over the diagram a read line will be displayed. Press down the left mouse button and drag to select the regions. To subtract a region again use the right mouse button instead. 
The second tab contains everything related to the reflections. By selecting a reflection in the table it becomes possible to modify it. It is possible to change the minimum and maximum of the set range, the guesses for the x and y value of the peak position and the guess for the full width at half maximum (fwhm).  Below the computed values are displayed.

#####Dataset info
On the right hand side all the meta data of the selected dataset is displayed. Selecting the checkboxes next to the items displays them in a new column in the datasets panel. 

#####Diffractogram 
Sows the diffractogram of the selected datasets. Below the diffractogram one can chose to display a diffractogram using all selected datasets or use a fixed intensity scale for display similar to the option of the image.

