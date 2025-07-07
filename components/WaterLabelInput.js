// components/WaterLabelInput.js - 紧凑的水质标签输入组件

import { useState } from 'react'

const WaterLabelInput = ({ currentLabel, onLabelUpdate }) => {
  const [isEditing, setIsEditing] = useState(false)
  const [inputValue, setInputValue] = useState(currentLabel || '')
  const [isCustom, setIsCustom] = useState(false)
  const [saving, setSaving] = useState(false)

  // 预设的水源标签选项
  const presetLabels = [
    'Tap Water',
    'Brita Filtered Water',
    'Bottled Water',
    'Well Water',
    'Spring Water',
    'Distilled Water',
    'RO Filtered Water',
    'Lake Water',
    'Custom...'
  ]

  const handleSave = async () => {
    if (!inputValue.trim()) {
      alert('Please enter a water source label')
      return
    }

    try {
      setSaving(true)
      
      const response = await fetch('/api/update-water-label', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ 
          label: inputValue.trim() 
        }),
      })

      const result = await response.json()
      
      if (result.success) {
        onLabelUpdate(inputValue.trim())
        setIsEditing(false)
        setIsCustom(false)
      } else {
        throw new Error(result.message || 'Failed to update label')
      }
    } catch (error) {
      console.error('Error updating label:', error)
      alert('Failed to update water source label: ' + error.message)
    } finally {
      setSaving(false)
    }
  }

  const handleCancel = () => {
    setInputValue(currentLabel || '')
    setIsEditing(false)
    setIsCustom(false)
  }

  const handleSelectChange = (e) => {
    const value = e.target.value
    if (value === 'Custom...') {
      setIsCustom(true)
      setInputValue('')
    } else {
      setIsCustom(false)
      setInputValue(value)
    }
  }

  return (
    <div className="compact-water-label">
      <div className="label-header-compact">
        <span className="label-icon">🏷️</span>
        <span className="label-title">Water Source</span>
      </div>

      {!isEditing ? (
        <div className="label-display-compact">
          <div className="current-label-compact">
            {currentLabel || 'Unlabeled Sample'}
          </div>
          <button 
            onClick={() => setIsEditing(true)}
            className="edit-button-compact"
            title="Edit water source"
          >
            ✏️
          </button>
        </div>
      ) : (
        <div className="label-edit-compact">
          {!isCustom ? (
            <select 
              value={inputValue}
              onChange={handleSelectChange}
              className="label-select"
              autoFocus
            >
              <option value="">Select water source...</option>
              {presetLabels.map((label, index) => (
                <option key={index} value={label}>
                  {label}
                </option>
              ))}
            </select>
          ) : (
            <input
              type="text"
              value={inputValue}
              onChange={(e) => setInputValue(e.target.value)}
              placeholder="Enter custom water source..."
              className="label-input-compact"
              maxLength={50}
              autoFocus
            />
          )}
          
          <div className="action-buttons-compact">
            <button 
              onClick={handleSave}
              disabled={saving || !inputValue.trim()}
              className="save-button-compact"
            >
              {saving ? '💾' : '✓'}
            </button>
            <button 
              onClick={handleCancel}
              className="cancel-button-compact"
              disabled={saving}
            >
              ✕
            </button>
          </div>
        </div>
      )}
    </div>
  )
}

export default WaterLabelInput