// components/Footer.js - Contact Us按钮移至右下角的页脚组件

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
            {/* 左侧版权信息 */}
            <div className="footer-info">
              <p>&copy; 2025 AquaMonitor - Making Every Drop Trustworthy</p>
              <p className="tech-stack">Powered by IoT & LoRaWAN Technology • Real-time Sensor Data</p>
            </div>
            
            {/* 右侧Contact Us按钮 */}
            <div className="footer-contact">
              <a 
                href="#contact" 
                className="footer-contact-link" 
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