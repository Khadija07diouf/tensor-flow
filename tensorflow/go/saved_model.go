/*
Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

package tensorflow

import (
	"runtime"
	"unsafe"
)

// #include <stdlib.h>
// #include "tensorflow/c/c_api.h"
import "C"

// SavedModel represents the contents of loaded SavedModel.
// TODO(jhseu): Add and document metagraphdef when we pregenerate protobufs.
type SavedModel struct {
	Session *Session
	Graph   *Graph
}

// LoadSavedModel creates a new SavedModel from a model previously
// exported to a directory on disk.
//
// Exported models contain a set of graphs and, optionally, variable values.
// Tags in the model identify a single graph. LoadSavedModel initializes a
// session with the identified graph and with variables initialized to from the
// checkpoints on disk.
//
// The tensorflow package currently does not have the ability to export a model
// to a directory from Go. This function thus currently targets loading models
// exported in other languages, such as using tf.saved_model.builder in Python.
// See:
// https://www.tensorflow.org/code/tensorflow/python/saved_model/
func LoadSavedModel(exportDir string, tags []string, options *SessionOptions, runOptions *[]byte, metaGraph *[]byte) (*SavedModel, error) {
	status := newStatus()
	cOpt, doneOpt, err := options.c()
	defer doneOpt()
	if err != nil {
		return nil, err
	}
	cExportDir := C.CString(exportDir)
	cTags := make([]*C.char, len(tags))
	for i := range tags {
		cTags[i] = C.CString(tags[i])
	}
	graph := NewGraph()
	
	var tfRunOptions *C.TF_Buffer
	if runOptions != nil {
		data := C.CBytes(*runOptions)
		defer C.free(data)

		tfRunOptions = C.TF_NewBuffer()
		tfRunOptions.data = data
		tfRunOptions.length = C.size_t(len(*runOptions))
		defer C.TF_DeleteBuffer(tfRunOptions)
	} else {
		tfRunOptions = nil
	}

	var tfMetaGraph *C.TF_Buffer
	if (metaGraph != nil){
		data := C.CBytes(*metaGraph)
		defer C.free(data)

		tfMetaGraph = C.TF_NewBuffer()
		tfMetaGraph.data = data
		tfMetaGraph.length = C.size_t(len(*metaGraph))
		defer C.TF_DeleteBuffer(tfMetaGraph)
	} else{
		metaGraph = nil
	}
	
	// TODO(jhseu): Add support for run_options and meta_graph_def.
	cSess := C.TF_LoadSessionFromSavedModel(cOpt, tfRunOptions, cExportDir, (**C.char)(unsafe.Pointer(&cTags[0])), C.int(len(cTags)), graph.c, tfMetaGraph, status.c)
	for i := range cTags {
		C.free(unsafe.Pointer(cTags[i]))
	}
	C.free(unsafe.Pointer(cExportDir))

	if err := status.Err(); err != nil {
		return nil, err
	}
	s := &Session{c: cSess}
	runtime.SetFinalizer(s, func(s *Session) { s.Close() })
	return &SavedModel{Session: s, Graph: graph}, nil
}
