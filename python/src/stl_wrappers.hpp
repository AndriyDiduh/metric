/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.

  Copyright (c) 2020 Panda Team
*/

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>
#include <type_traits>

/**
    Tiny adapter that provides NumpyArray with std::vector like interface
*/
template<typename T, int Dims = 1>
class NumpyToVectorAdapter: public pybind11::array_t<T> {
    using Accessor = pybind11::detail::unchecked_reference<T, Dims>;
    std::unique_ptr<Accessor> internalBuffer;
public:
    using Iterator = T*;

    using pybind11::array_t<T>::array_t;

    NumpyToVectorAdapter(pybind11::array_t<T> obj)
        : pybind11::array_t<T>(obj)
    {
        this->internalBuffer(new Accessor(this->template unchecked<T, Dims>()));
    }

    T operator[](size_t index) const {
        return this->internalBuffer->operator[](index);
    }

    bool empty() const {
        return this->size() == 0;
    }

    Iterator begin() const {
        return pybind11::detail::array_begin<T>(this->request());
    }

    Iterator end() const {
        return pybind11::detail::array_end<T>(this->request());
    }
};

template<typename T, int Dims = 2>
class NumpyToMatrixAdapter: public pybind11::array_t<T> {
    using Accessor = pybind11::detail::unchecked_reference<T, Dims>;

    template<typename T1>
    class Proxy {
        Accessor* buffer;
        size_t dimension;
    public:
        using Iterator = typename std::add_pointer<T1>::type;

        Proxy(Accessor* buffer, size_t dimension)
            : buffer(buffer)
            , dimension(dimension)
        {
        }

        const T1& operator[](size_t index) const {
            return this->buffer->operator()(dimension, index);
        }

        bool empty() const {
            return this->size() == 0;
        }

        size_t size() const {
            return this->buffer->shape(dimension);
        }

        Iterator begin() const {
            return pybind11::detail::array_iterator<T>(this->buffer->data(dimension));
        }

        Iterator end() const {
            return pybind11::detail::array_iterator<T>(this->buffer->data(dimension) + this->size());
        }
    };

    std::unique_ptr<Accessor> internalBuffer;
public:
    NumpyToMatrixAdapter(pybind11::array_t<T> obj)
        : pybind11::array_t<T>(obj)
    {
        this->internalBuffer(new Accessor(this->template unchecked<T, Dims>()));
    }

    Proxy<T> operator[](size_t index) const {
        return Proxy<T>(this->internalBuffer.get(), index);
    }
};

//template<typename T>
//class WrapStlMatrix: public base_python_object {
//public:
//    typedef std::vector<T> value_type;
//    WrapStlMatrix() = default;
//    WrapStlMatrix(base_python_object& obj)
//        : base_python_object(obj) {
//    }
//
//    size_t size() const {
//        return boost::python::len(*this);
//    }
//
//    bool empty() const {
//        return size() == 0;
//    }
//
//	boost::python::stl_input_iterator<T> begin() const {
//        return boost::python::stl_input_iterator<T>(*this);
//    }
//
//	boost::python::stl_input_iterator<T> end() const {
//        return boost::python::stl_input_iterator<T>();
//    }
//
//    WrapStlMatrix operator[](int index) const {
//        base_python_object wr = boost::python::extract<base_python_object>(base_python_object::operator[](index));
//        return WrapStlMatrix(wr);
//    }
//};
