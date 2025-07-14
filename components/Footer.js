// components/Footer.js - 优化后的简洁页脚组件（带Contact Us弹窗）

import { useState } from 'react'
import ContactModal from './ContactModal'

const Footer = () => {
  const [showContact, setShowContact] = useState(false)

  const handleContactClick = (e) => {
    e.preventDefault()
    setShowContact(true)
  }

  const handleCloseContact = () => {
    setShowContact(false)
  }

  return (
    <>
      <footer className="footer">
        <div className="container">
          <div className="footer-content">
            <div className="footer-info">
              <p>&copy; 2025 AquaMonitor - Making Every Drop Trustworthy</p>
              <p className="tech-stack">Powered by IoT & LoRaWAN Technology • Real-time Sensor Data</p>
            </div>
            
            {/* 简化为只有Contact Us链接 */}
            <div className="footer-links">
              <a 
                href="#contact" 
                className="footer-link" 
                onClick={handleContactClick}
              >
                Contact Us
              </a>
            </div>
          </div>
        </div>
      </footer>

      {/* Contact Us弹窗 */}
      <ContactModal 
        isOpen={showContact} 
        onClose={handleCloseContact} 
      />
    </>
  )
}

export default Footer