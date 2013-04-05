// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

////////////////////////////////////////////////////////////////////////
// Simple class to deal with smart locking/unlocking of python GIL
////////////////////////////////////////////////////////////////////////
%{

struct PyGILPhondler
{
    PyGILPhondler(void):
        s(PyGILState_Ensure())
    {
        //NOP
    }
    ~PyGILPhondler(void)
    {
        PyGILState_Release(s);
    }
    PyGILState_STATE s;
};

%}

////////////////////////////////////////////////////////////////////////
// Simple class to deal with smart save/restore of python thread state
////////////////////////////////////////////////////////////////////////
%{

struct PyTSPhondler
{
    PyTSPhondler(void):
        s(PyEval_SaveThread())
    {
        //NOP
    }
    ~PyTSPhondler(void)
    {
        PyEval_RestoreThread(s);
    }
    PyThreadState *s;
};

%}

////////////////////////////////////////////////////////////////////////
// Create a reference holder for python objects
////////////////////////////////////////////////////////////////////////
%inline %{

struct PyObjectRefHolder
{
    PyObjectRefHolder(PyObject *o):
        o(o)
    {
        Py_INCREF(o);
    }
    ~PyObjectRefHolder(void)
    {
        PyGILPhondler phil;
        Py_DECREF(o);
    }
    PyObject *o;
};

%}
