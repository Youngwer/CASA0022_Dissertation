// components/WaterLabelInput.js - 水质标签输入组件

import { useState } from 'react'

const WaterLabelInput = ({ currentLabel, onLabelUpdate }) => {
  const [isEditing, setIsEditing] = useState(false)
  const [inputValue, setInputValue] = useState(currentLabel || '')
  const [saving, setSaving] = useState(false)

  // 预设的水源标签建议
  const suggestionLabels = [
    'Tap Water',
    'Brita Filtered Water',
    'Bottled Water',
    'Well Water',
    'Spring Water',
    'Distilled Water',
    'RO Filtered Water',
    'Lake Water',
    'River Water',
    'Rainwater'
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
  }

  const handleSuggestionClick = (suggestion) => {
    setInputValue(suggestion)
  }

  return (
    <div className="water-label-container">
      <div className="label-header">
        <span className="label-icon">🏷️</span>
        <span className="label-title">Water Source</span>
      </div>

      {!isEditing ? (
        <div className="label-display">
          <span className="current-label">
            {currentLabel || 'Unlabeled Water Sample'}
          </span>
          <button 
            onClick={() => setIsEditing(true)}
            className="edit-button"
            title="Edit water source label"
          >
            ✏️ Edit
          </button>
        </div>
      ) : (
        <div className="label-edit">
          <div className="input-section">
            <input
              type="text"
              value={inputValue}
              onChange={(e) => setInputValue(e.target.value)}
              placeholder="e.g., Brita Filtered Water"
              className="label-input"
              maxLength={50}
              autoFocus
            />
            
            <div className="action-buttons">
              <button 
                onClick={handleSave}
                disabled={saving || !inputValue.trim()}
                className="save-button"
              >
                {saving ? '💾 Saving...' : '💾 Save'}
              </button>
              <button 
                onClick={handleCancel}
                className="cancel-button"
                disabled={saving}
              >
                ❌ Cancel
              </button>
            </div>
          </div>

          <div className="suggestions-section">
            <span className="suggestions-title">Quick Select:</span>
            <div className="suggestions-grid">
              {suggestionLabels.map((suggestion, index) => (
                <button
                  key={index}
                  onClick={() => handleSuggestionClick(suggestion)}
                  className="suggestion-button"
                >
                  {suggestion}
                </button>
              ))}
            </div>
          </div>
        </div>
      )}
    </div>
  )
}

export default WaterLabelInput