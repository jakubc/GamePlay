#ifndef MATERIALPARAMETER_H_
#define MATERIALPARAMETER_H_

#include "AnimationTarget.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix.h"
#include "Texture.h"
#include "Effect.h"

namespace gameplay
{

/**
 * Defines a material parameter.
 *
 * This class represents a parameter that can be set for a material.
 * The methods in this class provide a mechanism to set parameters
 * of all supported types. Some types support setting by value,
 * while others only support setting by reference/pointer.
 *
 * Setting a parameter by reference/pointer provides the ability to
 * pass an array of values as well as a convenient way to support
 * auto-binding of values to a material parameter. For example, by
 * setting the parameter value to a pointer to a Matrix, any changes
 * to the Matrix will automatically be reflected in the technique the
 * next time the parameter is applied to the render state.
 */
class MaterialParameter : public AnimationTarget, public Ref
{
    friend class RenderState;

public:

    /**
     * Animates the uniform.
     */
    static const int ANIMATE_UNIFORM = 1;

    /**
     * Returns the name of this material parameter.
     */
    const char* getName() const;

    /**
     * Sets the value of this parameter to a float value.
     */
    void setValue(float value);

    /**
     * Sets the value of this parameter to an integer value.
     */
    void setValue(int value);

    /**
     * Stores a pointer/array of float values in this parameter.
     */
    void setValue(const float* values, unsigned int count = 1);

    /**
     * Stores a pointer/array of integer values in this parameter.
     */
    void setValue(const int* values, unsigned int count = 1);

    /**
     * Stores a copy of the specified Vector2 value in this parameter.
     */
    void setValue(const Vector2& value);

    /**
     * Stores a pointer/array of Vector2 values in this parameter.
     */
    void setValue(const Vector2* values, unsigned int count = 1);

    /**
     * Stores a copy of the specified Vector3 value in this parameter.
     */
    void setValue(const Vector3& value);

    /**
     * Stores a pointer/array of Vector3 values in this parameter.
     */
    void setValue(const Vector3* values, unsigned int count = 1);

    /**
     * Stores a copy of the specified Vector4 value in this parameter.
     */
    void setValue(const Vector4& value);

    /**
     * Stores a pointer/array of Vector4 values in this parameter.
     */
    void setValue(const Vector4* values, unsigned int count = 1);

    /**
     * Stores a copy of the specified Matrix value in this parameter.
     */
    void setValue(const Matrix& value);

    /**
     * Stores a pointer/array of Matrix values in this parameter.
     */
    void setValue(const Matrix* values, unsigned int count = 1);

    /**
     * Sets the value of this parameter to the specified texture sampler.
     */
    void setValue(const Texture::Sampler* sampler);

    /**
     * Loads a texture sampler from the specified path and sets it as the value of this parameter.
     *
     * @param texturePath The path to the texture to set.
     * @param generateMipmaps True to generate a full mipmap chain for the texture, false otherwise.
     *
     * @return The texture sampler that was set for this material parameter.
     */
    Texture::Sampler* setValue(const char* texturePath, bool generateMipmaps);

    /**
     * Binds the return value of a class method to this material parameter.
     *
     * This method enables binding of arbitrary class methods to a material
     * parameter. This is useful when you want to set a material parameter
     * to a variable that is frequently changing (such as a world matrix).
     *
     * By binding a method pointer, the method will be called automatically
     * to retrieve the updated parameter value each time the material is bound
     * for rendering.
     *
     * @param classInstance The instance of the class containing the member method to bind.
     * @param valueMethod A pointer to the class method to bind (in the format '&class::method').
     */
    template <class ClassType, class ParameterType>
    void bindValue(ClassType* classInstance, ParameterType (ClassType::*valueMethod)() const);

    /**
     * Binds the return value of a class method to this material parameter.
     *
     * This overloads the setBinding method to provide support for array parameters.
     * The valueMethod parameter should return an array (pointer) of a supported
     * material parameter type, such as Matirx* for an array of matrices. The
     * countMethod should point to a method that returns the number of entries in
     * the value returned from valueMethod.
     *
     * @param classInstance The instance of the class containing the member method to bind.
     * @param valueMethod A pointer to the class method to bind (in the format '&class::method').
     * @param countMethod A pointer to a method that returns the number of entries in the array returned by valueMethod.
     */
    template <class ClassType, class ParameterType>
    void bindValue(ClassType* classInstance, ParameterType (ClassType::*valueMethod)() const, unsigned int (ClassType::*countMethod)() const);

    /**
     * @see AnimationTarget#getAnimationPropertyComponentCount
     */
    unsigned int getAnimationPropertyComponentCount(int propertyId) const;

    /**
     * @see AnimationTarget#getAnimationProperty
     */
    void getAnimationPropertyValue(int propertyId, AnimationValue* value);

    /**
     * @see AnimationTarget#setAnimationProperty
     */
    void setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight = 1.0f);

private:
   
    /**
     * Constructor.
     */
    MaterialParameter(const char* name);
    
    /**
     * Destructor.
     */
    ~MaterialParameter();

    /**
     * Hidden copy assignment operator.
     */
    MaterialParameter& operator=(const MaterialParameter&);
    
    /**
     * Interface implemented by templated method bindings for simple storage and iteration.
     */
    class MethodBinding : public Ref
    {
    public:
        virtual void setValue(Effect* effect) = 0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MethodBinding() { }

        /**
         * Hidden copy assignment operator.
         */
        MethodBinding& operator=(const MethodBinding&);
    };

    /**
     * Defines a method parameter binding for a single value.
     */
    template <class ClassType, class ParameterType>
    class MethodValueBinding : public MethodBinding
    {
        typedef ParameterType (ClassType::*ValueMethod)() const;
    public:
        MethodValueBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod);
        void setValue(Effect* effect);
    private:
        MaterialParameter* _parameter;
        ClassType* _instance;
        ValueMethod _valueMethod;

    };

    /**
     * Defines a method parameter binding for an array of values.
     */
    template <class ClassType, class ParameterType>
    class MethodArrayBinding : public MethodBinding
    {
        typedef ParameterType (ClassType::*ValueMethod)() const;
        typedef unsigned int (ClassType::*CountMethod)() const;
    public:
        MethodArrayBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod, CountMethod countMethod);
        void setValue(Effect* effect);
    private:
        MaterialParameter* _parameter;
        ClassType* _instance;
        ValueMethod _valueMethod;
        CountMethod _countMethod;
    };

    void clearValue();

    void bind(Effect* effect);

    void applyAnimationValue(AnimationValue* value, float blendWeight, int components);

    void cloneInto(MaterialParameter* materialParameter) const;
    
    union
    {
        float floatValue;
        int intValue;
        float* floatPtrValue;
        int* intPtrValue;
        const Texture::Sampler* samplerValue;
        MethodBinding* method;
    } _value;
    
    enum
    {
        NONE,
        FLOAT,
        INT,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        MATRIX,
        SAMPLER,
        METHOD
    } _type;
    
    unsigned int _count;
    bool _dynamic;
    std::string _name;
    Uniform* _uniform;
};

template <class ClassType, class ParameterType>
void MaterialParameter::bindValue(ClassType* classInstance, ParameterType (ClassType::*valueMethod)() const)
{
    clearValue();

    _value.method = new MethodValueBinding<ClassType, ParameterType>(this, classInstance, valueMethod);
    _dynamic = true;
    _type = MaterialParameter::METHOD;
}

template <class ClassType, class ParameterType>
void MaterialParameter::bindValue(ClassType* classInstance, ParameterType (ClassType::*valueMethod)() const, unsigned int (ClassType::*countMethod)() const)
{
    clearValue();

    _value.method = new MethodArrayBinding<ClassType, ParameterType>(this, classInstance, valueMethod, countMethod);
    _dynamic = true;
    _type = MaterialParameter::METHOD;
}

template <class ClassType, class ParameterType>
MaterialParameter::MethodValueBinding<ClassType, ParameterType>::MethodValueBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod) :
    _parameter(param), _instance(instance), _valueMethod(valueMethod)
{
}

template <class ClassType, class ParameterType>
void MaterialParameter::MethodValueBinding<ClassType, ParameterType>::setValue(Effect* effect)
{
    effect->setValue(_parameter->_uniform, (_instance->*_valueMethod)());
}

template <class ClassType, class ParameterType>
MaterialParameter::MethodArrayBinding<ClassType, ParameterType>::MethodArrayBinding(MaterialParameter* param, ClassType* instance, ValueMethod valueMethod, CountMethod countMethod) :
    _parameter(param), _instance(instance), _valueMethod(valueMethod), _countMethod(countMethod)
{
}

template <class ClassType, class ParameterType>
void MaterialParameter::MethodArrayBinding<ClassType, ParameterType>::setValue(Effect* effect)
{
    effect->setValue(_parameter->_uniform, (_instance->*_valueMethod)(), (_instance->*_countMethod)());
}

}

#endif